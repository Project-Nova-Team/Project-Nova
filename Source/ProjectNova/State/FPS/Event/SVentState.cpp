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

	bIsLerpingToCrawlPosition = true; // turns tick logic off while moving

	Vent = Cast<AVent>(Shooter->GetLastScannedObject());

	if (Vent)
	{
		if (Vent->GetSpline())
		{
			Spline = Vent->GetSpline(); // set spline
		}

		ProneOffset = 25;

		LerpToStandSpeed = Vent->GetLerpToStandingSpeed(); // how fast player stands up	

		LerpToCrawlSpeed = Vent->GetLerpToCrawlSpeed(); // how fast player goes into crawl

		CrawlSpeed = Vent->GetCrawlSpeed(); // how fast player crawls

		ProgressMax = Spline->GetSplineLength();

		Handle->GetAction()->StopAction();

		if (Vent->bIsOverlappingLeftTrigger)
		{
			CrawlDirection = ECrawlDirection::CD_Right;
			Progress = 0;
			StandingProgress = ProgressMax;
		}
		else if (Vent->bIsOverlappingRightTrigger)
		{
			CrawlDirection = ECrawlDirection::CD_Left;
			Progress = ProgressMax; // start at the end if going from right to left
			StandingProgress = 0;
		}
		TargetCrawlRotation = Spline->GetRotationAtDistanceAlongSpline(Progress, ESplineCoordinateSpace::World);

		TargetCrawlLocation = Spline->GetLocationAtDistanceAlongSpline(Progress, ESplineCoordinateSpace::World);

		TargetCrawlLocation.Z -= ProneOffset;

		TargetStandingLocation = FVector(Spline->GetLocationAtDistanceAlongSpline(StandingProgress, 
			ESplineCoordinateSpace::World).X, Spline->GetLocationAtDistanceAlongSpline(StandingProgress, 
				ESplineCoordinateSpace::World).Y, Shooter->GetActorLocation().Z);

		Handle = GetDelayedActionManager()->StartOverTimeAction( // start crawl
			this,
			&USVentState::LerpToCrawl,
			LerpToCrawlSpeed,
			Shooter->GetActorLocation(),
			TargetCrawlLocation,
			Shooter->GetAnchor()->GetComponentRotation(),
			TargetCrawlRotation);
	}
}

void USVentState::OnExit()
{
	Movement->bIsInTuckTransition = false; // so that we can crouch again

	if (bIsLerpingToStandingPosition)
		bIsLerpingToStandingPosition = false;

	Super::OnExit();
}

void USVentState::Tick(const float DeltaTime) 
{
	if (!bIsLerpingToCrawlPosition && !bIsLerpingToStandingPosition)
	{
		Progress = FMath::Clamp(Progress, 0.f, ProgressMax);

		MoveAlongSpline(CrawlDirection, DeltaTime);
		RotateAlongSpline();
	}
}

void USVentState::LerpToCrawl(FVector StartingPosition, FVector EndPosition, FRotator StartingRotation, FRotator EndRotation)
{
	if (Handle->CurrentActionProgress > .999f)
	{
		bIsLerpingToCrawlPosition = false;
	}

	Shooter->SetActorLocation(FMath::Lerp(StartingPosition, EndPosition, Handle->CurrentActionProgress));
	Shooter->GetAnchor()->SetWorldRotation(FMath::Lerp(StartingRotation, EndRotation, Handle->CurrentActionProgress));
}

void USVentState::LerpToStanding(FVector StartingPosition, FVector EndPosition)
{
	if (Handle->CurrentActionProgress > .999f)
	{
		Shooter->GetStateMachine()->SetState("Walking");

		Vent->ReEnableGrate();
	}

	Shooter->SetActorLocation(FMath::Lerp(StartingPosition, EndPosition, Handle->CurrentActionProgress));
}

void USVentState::MoveAlongSpline(ECrawlDirection Direction, float DeltaTime)
{
	if (Direction == ECrawlDirection::CD_Right)
	{
		if (Progress == ProgressMax)
		{
			LeaveCrawl(Shooter->GetActorLocation(), TargetStandingLocation);
		}
		Progress += Input->MoveY * CrawlSpeed * DeltaTime;		// progress going right is positive
	}
	else
	{
		if (Progress == 0)
		{
			LeaveCrawl(Shooter->GetActorLocation(), TargetStandingLocation);
		}
		Progress -= Input->MoveY * CrawlSpeed * DeltaTime;
	}

	LocationAtDistanceAlongSpline = Spline->GetLocationAtDistanceAlongSpline
	(Progress, ESplineCoordinateSpace::World);

	LocationAtDistanceAlongSpline.Z -= ProneOffset;

	Shooter->SetActorLocation(LocationAtDistanceAlongSpline);
}

void USVentState::RotateAlongSpline()
{
	Shooter->GetAnchor()->SetWorldRotation(TargetCrawlRotation);
}

void USVentState::LeaveCrawl(FVector StartingPosition, FVector EndPosition)
{
	Handle = GetDelayedActionManager()->StartOverTimeAction(
		this,
		&USVentState::LerpToStanding,
		LerpToStandSpeed,
		StartingPosition,
		EndPosition);

	bIsLerpingToStandingPosition = true;
}
