#include "SWalkState.h"

void USWalkState::Initialize(UStateMachine* StateMachine, UObject* ContextObject)
{
	Super::Initialize(StateMachine, ContextObject);

	MaxAppliedInputSpeed = Movement->WalkMaxSpeed;
	MoveAcceleration = Movement->WalkAcceleration;
}

void USWalkState::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
	CheckForRunState();
	CheckForCrouchState();
	CheckForProneState();
}

void USWalkState::CheckForRunState()
{
	//Not holding sprint button or not on ground. Or we are aiming
	if (!Input->bIsTryingToSprint || !Movement->bIsOnGround || Shooter->GetCombat()->GetIsAimed())
	{
		return;
	}

	//Not moving fast enough
	const float MinRunSpeedSquare = Movement->RunMinSpeed * Movement->RunMinSpeed;
	if (Movement->Velocity.SizeSquared2D() < MinRunSpeedSquare)
	{
		return;
	}

	//Not looking close enough to the direction of movement
	if ((Shooter->GetCamera()->GetForwardVector() | Movement->Velocity.GetSafeNormal2D()) < Movement->RunLookMoveMinDot)
	{
		return;
	}
	
	FlagTransition("Running", 1);
}

void USWalkState::CheckForCrouchState()
{
	if (Input->bIsTryingToCrouch && Movement->bIsOnGround && !Movement->bIsInTuckTransition)
	{
		FlagTransition("Crouching", 1);
	}
}

void USWalkState::CheckForProneState()
{
	if (Input->bIsTryingToProne && Movement->bIsOnGround && !Movement->bIsInTuckTransition)
	{
		FlagTransition("Proning", 1);
	}
}