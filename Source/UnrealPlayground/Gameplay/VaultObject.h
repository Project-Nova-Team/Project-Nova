// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractiveObject.h"
#include "VaultObject.generated.h"

UCLASS()
class UNREALPLAYGROUND_API AVaultObject : public AActor, public IInteractiveObject
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVaultObject();

private:
	virtual void InteractionEvent(const APawn* EventSender) override;

	virtual void SetInteractiveObjectHidden(bool ActiveState) override;
};
