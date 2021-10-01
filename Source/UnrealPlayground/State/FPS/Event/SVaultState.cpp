// Fill out your copyright notice in the Description page of Project Settings.

#include "SVaultState.h"

void USVaultState::OnEnter()
{
	Super::OnEnter();

	// Vault animation here

	Shooter->StartVaultAnimation();


	UE_LOG(LogTemp, Warning, TEXT("Good work. Go to bed now."));

	//Shooter->GetArms()->GetAnimInstance()->Montage_Play();
}

void USVaultState::OnExit()
{
	Super::OnExit();
}

void USVaultState::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void USVaultState::Initialize(UStateMachine* StateMachine, UObject* ContextObject)
{
	Super::Initialize(StateMachine, ContextObject);
}
