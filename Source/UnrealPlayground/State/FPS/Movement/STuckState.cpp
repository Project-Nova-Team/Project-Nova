#include "STuckState.h"
#include "../ShooterStateMachine.h"

void USTuckState::OnEnter()
{
	Super::OnEnter();
	Movement->bIsInTuckTransition = true;

	//HACK this assumes the only states that enter a tuck state are already movement states
	//This breaks if we want to move from an event state to a tuck state
	UShooterStateMachine* DownCastedMachine = Cast <UShooterStateMachine>(GetMachine());
	USMovementState* DownCastedPreviousState = Cast<USMovementState>(DownCastedMachine->GetPreviousState());
	StartHeightInterpolation(TargetHalfHeight, DownCastedPreviousState->ExpectedHalfHeight, TargetCameraHeight);
}

void USTuckState::OnExit()
{
	Super::OnExit();
	//If the state we are transitioning to is another type of tuck state we don't need to handle crouching/standing on exit
	const FString NewStateKey = GetFlaggedKey();
	const UState* NewStatePtr = GetFlaggedState();

	if (!NewStateKey.IsEmpty() && !GetMachine()->GetStateAtKey(NewStateKey)->IsA(USTuckState::StaticClass()))
	{
		StartHeightInterpolation(Movement->StandingHeight, TargetHalfHeight, Movement->CameraHeight);
	}
	if (NewStatePtr && !NewStatePtr->IsA(USTuckState::StaticClass()))
	{
		StartHeightInterpolation(Movement->StandingHeight, TargetHalfHeight, Movement->CameraHeight);
	}
}

void USTuckState::StartHeightInterpolation(const float NewColliderHalfHeight, const float ExpectedHeight, const float NewCameraHeight)
{
	const float CurrentHeight = Shooter->GetCollider()->GetScaledCapsuleHalfHeight();
	const float CameraHeight = Shooter->GetAnchor()->GetRelativeLocation().Z;
	float ActionTime = Movement->CrouchTime;

	//Don't bother resizing if we're already at the desired height
	if (NewColliderHalfHeight == CurrentHeight)
	{
		Movement->bIsInTuckTransition = false;
		return;
	}

	if (CurrentHeight != ExpectedHeight)
	{
		//Makes crouch time shorter if the collider has to re-size mid transition
		//This occurs when the shooter enters an invalid position while transitioning to that state
		ActionTime *= FMath::Abs((CurrentHeight - NewColliderHalfHeight) / (ExpectedHeight - NewColliderHalfHeight));

		if (ActionTime == 0.f)
		{
			Movement->bIsInTuckTransition = false;
			return;
		}
	}

	Handle = GetDelayedActionManager()->StartOverTimeAction(
		this,
		&USTuckState::AdjustHeight,
		ActionTime,
		CurrentHeight,
		NewColliderHalfHeight,
		CameraHeight,
		NewCameraHeight);
}

void USTuckState::AdjustHeight(const float StartHeight, const float TargetHeight, const float CameraStartHeight, const float TargetCamHeight)
{	
	//Resize the collider and snap the capsule component so the new bottom is in the same position as before
	const float NewColliderHalfHeight = FMath::Lerp(StartHeight, TargetHeight, Handle->CurrentActionProgress);
	const float HeightDifference = Shooter->GetCollider()->GetScaledCapsuleHalfHeight() - NewColliderHalfHeight;
	const FVector CapsuleLoc = Shooter->GetCollider()->GetComponentLocation();
	const FVector NewPosition = FVector(CapsuleLoc.X, CapsuleLoc.Y, CapsuleLoc.Z - HeightDifference);

	//We have entered a place that we cant resize into
	if (!CheckIfStandUpIsValid(TargetHeight))
	{
		//Stop the current height adjustment
		Handle->GetAction()->StopAction();
		Movement->bIsInTuckTransition = false;


		UState* PreviousState = Cast<UShooterStateMachine>(GetMachine())->GetPreviousState();
		if (PreviousState->IsA(USTuckState::StaticClass()))
		{
			GetMachine()->GetActiveState()->FlagTransition(PreviousState, 255);
		}
		else
		{
			GetMachine()->GetActiveState()->FlagTransition(this, 255);		
		}
	}

	//Otherwise this position is safe to resize into
	else
	{
		//Set the size and position
		Shooter->GetCollider()->SetCapsuleHalfHeight(NewColliderHalfHeight);	
		Shooter->GetCollider()->SetWorldLocation(NewPosition);

		//Calculate and set the new position of the camera joint
		const float NewCameraHeight = FMath::Lerp(CameraStartHeight, TargetCamHeight, Handle->CurrentActionProgress);
		const FVector NewCameraPos = FVector(0, 0, NewCameraHeight);

		Shooter->GetAnchor()->SetRelativeLocation(NewCameraPos);

		if (Handle->CurrentActionProgress >= 1.f)
		{
			Movement->bIsInTuckTransition = false;
		}
	}
}

bool USTuckState::CheckIfStandUpIsValid(const float NewColliderHalfHeight) const
{
	//Prep sweep data structs
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Shooter);

	const float SweepRadius = Shooter->GetCollider()->GetScaledCapsuleRadius() - KINDA_SMALL_NUMBER;
	const FCollisionShape Capsule = FCollisionShape::MakeCapsule(SweepRadius, NewColliderHalfHeight);

	//Calculate the position of the sweep
	const float CurrentHalfHeight = Shooter->GetCollider()->GetScaledCapsuleHalfHeight();
	const FVector CapsuleLoc = Shooter->GetCollider()->GetComponentLocation();
	FVector SweepPosition = FVector(CapsuleLoc.X, CapsuleLoc.Y, CapsuleLoc.Z - CurrentHalfHeight);
	SweepPosition.Z += NewColliderHalfHeight;

#if WITH_EDITOR
	if (Shooter->bTraceDebug)
		DrawDebugCapsule(Shooter->GetWorld(), SweepPosition, NewColliderHalfHeight, SweepRadius, FQuat::Identity, FColor::Green, false, 0.016f);
#endif

	//Perform the sweep
	return !Shooter->GetWorld()->SweepSingleByChannel(Hit, SweepPosition, SweepPosition, FQuat::Identity, ECC_Visibility, Capsule);
}