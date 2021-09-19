// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractiveObject.h"
#include "HealthPickup.generated.h"

class UHealthComponent;

UCLASS()
class UNREALPLAYGROUND_API AHealthPickup : public AActor, public IInteractiveObject
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHealthPickup();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void InteractionEvent(const APawn* EventSender) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
		int HealAmount;

};
