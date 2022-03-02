// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractiveObject.h"
#include <Runtime/Engine/Classes/Components/BoxComponent.h>
#include "VaultObject.generated.h"


UCLASS()
class PROJECTNOVA_API AVaultObject : public AActor, public IInteractiveObject
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVaultObject();

	void InteractionEvent(APawn* EventSender) override;

	bool CanInteract() const override { return bIsPlayerInTrigger; }

	FInteractionPrompt& GetInteractionPrompt() override { return Prompt; }

	FName& GetInteractionMappingName() override { return ActionMappingName; }

	void RecieveLookedAt(APawn* EventSender) override;

	void RecieveLookedAway(APawn* EventSender, int32 MappingIndexToRemove) override;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Offset;

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FInteractionPrompt Prompt;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ObjectBody;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* Trigger;

	/** Invoked by ActorBeginOverlap*/
	UFUNCTION()
	virtual void ActorStartOverlap(AActor* OverlappedActor, AActor* OtherActor);

	/** Invoked by ActorEndOverlap*/
	UFUNCTION()
	virtual void ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor);

	bool bIsPlayerInTrigger;

	/** Is set to interact by default. See Edit->ProjectSettings->Input for list of action mapping names.*/
	UPROPERTY(EditAnywhere, Category = "Interaction")
	FName ActionMappingName;
};
