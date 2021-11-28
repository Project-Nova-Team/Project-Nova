// Fill out your copyright notice in the Description page of Project Settings.
#include "PushButton.h"
#include "../ShooterGameMode.h"
#include "../Utility/DelayedActionManager.h"

// Called when the game starts or when spawned
void APushButton::BeginPlay()
{
	Super::BeginPlay();

	MeshComponent = FindComponentByClass<UStaticMeshComponent>();
}

void APushButton::InteractionEvent(APawn* EventSender)
{
	Handle = GetWorld()->GetAuthGameMode<AShooterGameMode>()->GetDelayedActionManager()->StartOverTimeAction(
		this, &APushButton::OverTimeTransition, ButtonTransitionTime);
}

void APushButton::OverTimeTransition()
{
	bIsMoving = true;

	SetActorRelativeLocation(GetActorLocation() *
		FMath::Lerp(0.f, PushDepth, Handle->CurrentActionProgress));
}

