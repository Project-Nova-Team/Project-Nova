// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SEventState.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "SVaultState.generated.h"

/**
 * 
 */
UCLASS()
class UNREALPLAYGROUND_API USVaultState : public USEventState
{
	GENERATED_BODY()

public:
	USVaultState() {}
	~USVaultState() {}

	void OnEnter() override; 
	void OnExit() override;

	void Tick(const float DeltaTime) override;

	void Initialize(UStateMachine* StateMachine, UObject* ContextObject) override;
};
