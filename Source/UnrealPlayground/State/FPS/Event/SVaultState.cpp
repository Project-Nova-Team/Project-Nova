// Fill out your copyright notice in the Description page of Project Settings.

#include "SVaultState.h"

void USVaultState::OnEnter()
{
	Super::OnEnter();

	// Broadcasts event for shooter anim blueprint that triggers Montage_Play
	ShooterAnimInstance->BroadcastVaultEvent();
}

void USVaultState::OnExit()
{
	Super::OnExit();
}

void USVaultState::Initialize(UStateMachine* StateMachine, UObject* ContextObject)
{
	Super::Initialize(StateMachine, ContextObject);

	ShooterAnimInstance = Cast<UShooterAnimInstance>(Shooter->GetSkeletalMeshComponent()->GetAnimInstance());
}
