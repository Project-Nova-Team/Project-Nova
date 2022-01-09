// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractiveObject.h"
#include "VaultObject.generated.h"

UCLASS()
class PROJECTNOVA_API AVaultObject : public AActor, public IInteractiveObject
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVaultObject();

	FInteractionPrompt& GetInteractionPrompt() override { return Prompt; }

private:
	virtual void InteractionEvent(APawn* EventSender) override;

	FInteractionPrompt Prompt;
};
