#include "SVentState.h"
#include "../../../State/FPS/ShooterStateMachine.h"
#include "../../../Gameplay/Vent.h"

void USVentState::Initialize(UStateMachine* StateMachine, UObject* ContextObject)
{
	Super::Initialize(StateMachine, ContextObject);
}

void USVentState::OnEnter()
{
	Super::OnEnter();

	Vent = Cast<AVent>(Shooter->GetLastScannedObject());

	if (Vent)
	{
		if (Vent->GetSpline())
		{
			Spline = Vent->GetSpline();
		}

		ProgressMax = Spline->GetSplineLength();

		bLerpingToCrawlPosition = true;

		Handle->GetAction()->StopAction();

		TargetLerpRotation = Spline->GetRotationAtDistanceAlongSpline(Progress, ESplineCoordinateSpace::World);

		if (Vent->bIsOverlappingLeftTrigger)
		{
			CrawlDirection = ECrawlDirection::CD_Right;

			Progress = 0;
		}
		else if (Vent->bIsOverlappingRightTrigger)
		{
			CrawlDirection = ECrawlDirection::CD_Left;

			Progress = ProgressMax; // start at the end if going from left to right

			TargetLerpRotation *= -1; // flip rotation for right to left
		}

		TargetLerpLocation = Spline->GetLocationAtDistanceAlongSpline(Progress, ESplineCoordinateSpace::World);

		Handle = GetDelayedActionManager()->StartOverTimeAction(
			this,
			&USVentState::MoveToCrawlPosition,
			2.f,
			Shooter->GetActorLocation(),
			TargetLerpLocation,
			Shooter->GetAnchor()->GetComponentRotation(),
			TargetLerpRotation);
	}
}

void USVentState::OnExit()
{
	Movement->bIsInTuckTransition = false; // so that we can crouch again
	Super::OnExit();
}

void USVentState::Tick(const float DeltaTime) 
{
	if (!bLerpingToCrawlPosition)
	{
		Progress = FMath::Clamp(Progress, 0.f, ProgressMax);

		MoveAlongSpline(CrawlDirection, DeltaTime);
		RotateAlongSpline(CrawlDirection);
	}
}

void USVentState::MoveToCrawlPosition(FVector StartingPosition, FVector EndPosition, FRotator StartingRotation, FRotator EndRotation)
{
	if (Handle->CurrentActionProgress > .999f)
	{
		bLerpingToCrawlPosition = false;
	}

	Shooter->SetActorLocation(FMath::Lerp(StartingPosition, EndPosition, Handle->CurrentActionProgress));
	Shooter->GetAnchor()->SetWorldRotation(FMath::Lerp(StartingRotation, EndRotation, Handle->CurrentActionProgress));
}

void USVentState::MoveAlongSpline(ECrawlDirection Direction, float DeltaTime)
{
	if (Direction == ECrawlDirection::CD_Right)
	{
		if (Progress == ProgressMax)
		{
			Shooter->GetStateMachine()->SetState("Walking");
		}
		Progress += Input->MoveY * Movement->CrawlSpeed * DeltaTime;		// progress going right is positive
	}
	else
	{
		if (Progress == 0)
		{
			Shooter->GetStateMachine()->SetState("Walking");
		}
		Progress -= Input->MoveY * Movement->CrawlSpeed * DeltaTime;
	}

	LocationAtDistanceAlongSpline = Spline->GetLocationAtDistanceAlongSpline
	(Progress, ESplineCoordinateSpace::World);

	Shooter->SetActorLocation(LocationAtDistanceAlongSpline);
}

void USVentState::RotateAlongSpline(ECrawlDirection Direction)
{
	Shooter->GetAnchor()->SetWorldRotation(TargetLerpRotation);
}
