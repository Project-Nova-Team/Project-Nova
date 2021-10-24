// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractiveObject.h"
#include "AmmoPickup.generated.h"

class UShooterCombatComponent;
class AGun;

UCLASS()
class UNREALPLAYGROUND_API AAmmoPickup : public AActor, public IInteractiveObject
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAmmoPickup();

	virtual void InteractionEvent(const APawn* EventSender) override;

protected:
	UPROPERTY(EditAnywhere)
		int AmmoAmount;
};
