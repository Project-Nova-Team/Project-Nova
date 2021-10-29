// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BarkTrigger.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FShowBarkEvent, FString, PrefixText, FString, SuffixText);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHideBarkEvent);

class UUserWidget;

UCLASS()
class UNREALPLAYGROUND_API ABarkTrigger : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABarkTrigger();

private:
	void ShowUIPrompt();

	void HideUIPrompt();

	// Overlap
	UFUNCTION()
	void EnterOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult);

	UFUNCTION()
	void ExitOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex);

	UPROPERTY(BlueprintAssignable)
	FShowBarkEvent ShowBarkEvent;

	UPROPERTY(BlueprintAssignable)
	FHideBarkEvent HideBarkEvent;
};
