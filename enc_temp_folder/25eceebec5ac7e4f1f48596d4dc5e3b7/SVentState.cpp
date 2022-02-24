// Fill out your copyright notice in the Description page of Project Settings.

#include "SVentState.h"
#include "../../../State/FPS/ShooterStateMachine.h"
#include "../../../Gameplay/Vent.h"
#include <ProjectNova/State/FPS/Movement/SProneState.h>
#include <time.h>

void USVentState::Initialize(UStateMachine* StateMachine, UObject* ContextObject)
{
	Super::Initialize(StateMachine, ContextObject);
}

void USVentState::OnEnter()
{
	UE_LOG(LogTemp, Warning, TEXT("Vent State Enter"));
	Super::OnEnter();

	Vent = Cast<AVent>(Shooter->GetLastScannedObject());

	if (Vent)
	{
		if (Vent->GetSpline())
		{
			Spline = Vent->GetSpline();
		}

		ProgressMax = Spline->GetSplineLength();

		SpeedAlongSpline = Vent->CrawlSpeed;

		bMovingToCrawl = true;

		Handle->GetAction()->StopAction();

		if (Vent->bIsOverlappingLeftTrigger)
		{
			bIsDirectionPositive = true;
			Progress = 0;

			UE_LOG(LogTemp, Warning, TEXT("Left Setup"));

			Handle = GetDelayedActionManager()->StartOverTimeAction(
				this,
				&USVentState::MoveToCrawlPosition,
				2.f,
				Shooter->GetActorLocation(),
				Spline->GetLocationAtDistanceAlongSpline(Progress, ESplineCoordinateSpace::World),
				Shooter->GetAnchor()->GetComponentRotation(),
				Spline->GetRotationAtDistanceAlongSpline(Progress, ESplineCoordinateSpace::World));
		}
		else if (Vent->bIsOverlappingRightTrigger)
		{
			bIsDirectionPositive = false;
			Progress = ProgressMax;

			UE_LOG(LogTemp, Warning, TEXT("Right Setup"));

			Handle = GetDelayedActionManager()->StartOverTimeAction(
				this,
				&USVentState::MoveToCrawlPosition,
				2.f,
				Shooter->GetActorLocation(),
				Spline->GetLocationAtDistanceAlongSpline(Progress, ESplineCoordinateSpace::World),
				Shooter->GetAnchor()->GetComponentRotation(),
				-1 * Spline->GetRotationAtDistanceAlongSpline(Progress, ESplineCoordinateSpace::World));
		}
	}
}

void USVentState::OnExit()
{
	UE_LOG(LogTemp, Warning, TEXT("Vent State Exit"));
}

void USVentState::Tick(const float DeltaTime) 
{
	if (!bMovingToCrawl)
	{
		Progress = FMath::Clamp(Progress, 0.f, ProgressMax);

		if (bIsDirectionPositive)
		{
			if (Progress == ProgressMax)
			{
				Shooter->GetStateMachine()->SetState("Walking");
			}

			Progress += Input->MoveY * SpeedAlongSpline;
			LocationAlongSpline = (Spline->GetLocationAtDistanceAlongSpline(Progress, ESplineCoordinateSpace::World));
			Shooter->GetAnchor()->SetWorldRotation(Spline->GetDirectionAtDistanceAlongSpline(Progress, ESplineCoordinateSpace::World).Rotation());
		}
		else
		{
			if (Progress == 0)
			{
				Shooter->GetStateMachine()->SetState("Walking");
			}
			Progress -= Input->MoveY * SpeedAlongSpline;
			LocationAlongSpline = (Spline->GetLocationAtDistanceAlongSpline(Progress, ESplineCoordinateSpace::World));
			Shooter->GetAnchor()->SetWorldRotation((-1 * Spline->GetDirectionAtDistanceAlongSpline(Progress, ESplineCoordinateSpace::World)).Rotation());
		}

		Shooter->SetActorLocation(LocationAlongSpline);
	}
}

void USVentState::MoveToCrawlPosition(FVector StartingPosition, FVector EndPosition, FRotator StartingRotation, FRotator EndRotation)
{
	if (Handle->CurrentActionProgress > .999f)
	{
		bMovingToCrawl = false;
	}

	Shooter->SetActorLocation(FMath::Lerp(StartingPosition, EndPosition, Handle->CurrentActionProgress));
	Shooter->GetAnchor()->SetWorldRotation(FMath::Lerp(StartingRotation, EndRotation, Handle->CurrentActionProgress));
}