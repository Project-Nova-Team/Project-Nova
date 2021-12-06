// Fill out your copyright notice in the Description page of Project Settings.
#include "InteractiveButton.h"
#include "../ShooterGameMode.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/AudioComponent.h"
#include "../Utility/DelayedActionManager.h"

AInteractiveButton::AInteractiveButton()
{
	PushDepth = 10.f;
	ButtonTransitionTime = .25f;

	Panel = CreateDefaultSubobject<UStaticMeshComponent>("Panel");
	SetRootComponent(Panel);

	Button = CreateDefaultSubobject<UStaticMeshComponent>("Button");
	Button->AttachToComponent(Panel, FAttachmentTransformRules::KeepRelativeTransform);

	AudioComponent = CreateDefaultSubobject<UAudioComponent>("Audio");
	AudioComponent->AttachToComponent(Panel, FAttachmentTransformRules::KeepRelativeTransform);

	Arrow = CreateDefaultSubobject<UArrowComponent>("Arrow");
	Arrow->AttachToComponent(Button, FAttachmentTransformRules::KeepRelativeTransform);

	//Dynamic Material for Button is being set in BP
}

void AInteractiveButton::SetIsLocked(const bool Value)
{
	if (Value != bIsLocked)
	{
		bIsLocked = Value;
		SetIsLockedImpl(Value);
	}
}

void AInteractiveButton::SetIsLockedImpl(const bool Value)
{
	FEditorScriptExecutionGuard ScriptGuard; //lets us call this function during edit time;
	{
		ReceiveLockStatusChange(Value);
	}

	MaybeChangeButtonState();
}

void AInteractiveButton::InteractionEvent(APawn* EventSender)
{
	MaybeChangeButtonState();
}


void AInteractiveButton::BeginPlay()
{
	Super::BeginPlay();
	InitialOffset = Button->GetRelativeLocation();
}

void AInteractiveButton::MaybeChangeButtonState()
{
	// if not locked and current state is extended, retract
	if (ShouldRetract())
	{
		State = EBS_Changing;
		StartDelayedAction(EBS_Retracted);
	}
	// if not locked and current state is retracted, extend
	else if (ShouldExtend())
	{
		State = EBS_Changing;
		StartDelayedAction(EBS_Extended);
	}
}

void AInteractiveButton::OverTimeTransition(const EButtonState TargetState)
{
	bIsMoving = true;

	const FVector Direction = FVector::ForwardVector;

	float Offset;

	if (TargetState == EBS_Retracted)
	{
		Offset = FMath::Lerp(0.f, PushDepth, Handle->CurrentActionProgress);
		Button->SetRelativeLocation(InitialOffset + Direction * Offset);
	}
	else
	{
		Offset = FMath::Lerp(PushDepth, 0.f, Handle->CurrentActionProgress);
		Button->SetRelativeLocation(InitialOffset + Direction * Offset);
	}

	/*We've finished transitioning. Decide if we should start
	closing or opening again because something might have occured during the transition*/
	if (Handle->CurrentActionProgress >= 1.f)
	{
		bIsMoving = false;

		State = TargetState;

		if (State == EBS_Retracted)
		{
			AudioComponent->Play();
			StartDelayedAction(EBS_Extended);
		}
	}
}

void AInteractiveButton::Retract()
{

}

void AInteractiveButton::Extend()
{
}

void AInteractiveButton::StartDelayedAction(EButtonState TargetState)
{
	Handle = GetWorld()->GetAuthGameMode<AShooterGameMode>()->GetDelayedActionManager()->StartOverTimeAction(
		this, &AInteractiveButton::OverTimeTransition, ButtonTransitionTime, TargetState);
}