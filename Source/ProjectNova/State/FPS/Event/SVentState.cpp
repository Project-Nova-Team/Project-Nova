// Fill out your copyright notice in the Description page of Project Settings.

#include "SVentState.h"
#include "../../../State/FPS/ShooterStateMachine.h"
#include "../../../Gameplay/Vent.h"
#include <ProjectNova/State/FPS/Movement/SProneState.h>

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

		if (Vent->bIsOverlappingLeftTrigger)
		{
			bIsDirectionPositive = true;
			Progress = 0;
		}
		else if (Vent->bIsOverlappingRightTrigger)
		{
			bIsDirectionPositive = false;
			Progress = ProgressMax;
		}

		SpeedAlongSpline = Vent->CrawlSpeed;

		Handle = GetDelayedActionManager()->StartOverTimeAction(
			this,
			&USVentState::MoveToCrawlPosition, 
			2.f,
			Shooter->GetActorLocation(),
			Spline->GetLocationAtDistanceAlongSpline(ProgressMax, ESplineCoordinateSpace::World));
	}
}

void USVentState::OnExit()
{
	UE_LOG(LogTemp, Warning, TEXT("Vent State Exit"));
}

void USVentState::Tick(const float DeltaTime) 
{
	if (Handle && Handle->bIsComplete)
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

void USVentState::MoveToCrawlPosition(FVector StartPosition, FVector EndPosition)
{
	if (Handle->CurrentActionProgress >= 1.f)
	{
		Handle->bIsComplete;
	}
	/*UE_LOG(LogTemp, Warning, TEXT("%s"), *StartPosition.ToString());
	UE_LOG(LogTemp, Warning, TEXT("%s"), *EndPosition.ToString());*/
	const FVector NewLocation = FMath::Lerp(StartPosition, EndPosition, Handle->CurrentActionProgress);
	Shooter->SetActorLocation(NewLocation);

}