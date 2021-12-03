// Fill out your copyright notice in the Description page of Project Settings.
#include "InteractiveButton.h"
#include "../ShooterGameMode.h"
#include "../Utility/DelayedActionManager.h"

AInteractiveButton::AInteractiveButton() 
{
	PushDepth = 4.5f;
	ButtonTransitionTime = .2f;
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
	// if not locked and current state is extended, retract
	if (ShouldRetract())
	{
		State = EBS_Changing;
		Retract();
	}
	// if not locked and current state is retracted, extend
	else if (ShouldExtend())
	{
		State = EBS_Changing;
		Extend();
	}
}

void AInteractiveButton::OverTimeTransition(const EButtonState TargetState)
{
	bIsMoving = true;

	const FVector Direction = FVector::RightVector;

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
	// SET TO .8 INSTEAD OF 1 because of async button not finishing movement on time issue. Doesn't fix it but helps
	if (Handle->CurrentActionProgress >= .8f)
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