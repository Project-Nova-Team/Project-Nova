// Fill out your copyright notice in the Description page of Project Settings.
#include "InteractiveButton.h"
#include "../ShooterGameMode.h"
#include "../Utility/DelayedActionManager.h"
#include <Runtime/Engine/Classes/Kismet/KismetMathLibrary.h>


AInteractiveButton::AInteractiveButton() 
{
	PushDepth = 2.f;
	ButtonTransitionTime = .25f;
}

void AInteractiveButton::SetIsLocked(const bool Value)
{
	if (Value != bIsLocked)
	{
		bIsLocked = Value;
	}
}

void AInteractiveButton::InteractionEvent(APawn* EventSender)
{
	MaybeChangeButtonState();
}

void AInteractiveButton::MaybeChangeButtonState()
{
	if (ShouldRetract())
	{
		State = EBS_Changing;
		Retract();
	}

	else if (ShouldExtend())
	{
		State = EBS_Changing;
		Extend();
	}
}

void AInteractiveButton::OverTimeTransition(const EButtonState TargetState)
{
	bIsMoving = true;

	/*This UKismetLibrary Function takes into account rotation, so that our 
	Right vector is always right no matter the button orientation**/ 
	const FVector Direction = UKismetMathLibrary::GetForwardVector(GetActorRotation());

	float Offset;

	if (TargetState == EBS_Extended)
	{
		Offset = FMath::Lerp(0.f, PushDepth, Handle->CurrentActionProgress);
		Mesh->SetRelativeLocation(Mesh->GetRelativeLocation() + -Direction * Offset);
	}
	else
	{
		Offset = FMath::Lerp(PushDepth, 0.f, Handle->CurrentActionProgress);
		Mesh->SetRelativeLocation(Mesh->GetRelativeLocation() + Direction * Offset);
	}

	/*We've finished transitioning. Decide if we should start
	closing or opening again because something might have occured during the transition*/
	if (Handle->CurrentActionProgress >= 1.f)
	{
		bIsMoving = false;

		State = TargetState;

		if(State == EBS_Retracted)
			Extend();
	}
}


void AInteractiveButton::Retract()
{
	Handle = GetWorld()->GetAuthGameMode<AShooterGameMode>()->GetDelayedActionManager()->StartOverTimeAction(
		this, &AInteractiveButton::OverTimeTransition, ButtonTransitionTime, EBS_Retracted);
}

void AInteractiveButton::Extend()
{
	Handle = GetWorld()->GetAuthGameMode<AShooterGameMode>()->GetDelayedActionManager()->StartOverTimeAction(
		this, &AInteractiveButton::OverTimeTransition, ButtonTransitionTime, EBS_Extended);
}