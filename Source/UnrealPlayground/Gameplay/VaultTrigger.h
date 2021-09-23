// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VaultTrigger.generated.h"

class UBoxComponent;
class UArrowComponent;

UCLASS()
class UNREALPLAYGROUND_API AVaultTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVaultTrigger();

private:
	UPROPERTY(VisibleAnywhere)
		UBoxComponent *Collider;
	UPROPERTY(VisibleAnywhere)
		UArrowComponent *Arrow;

};
