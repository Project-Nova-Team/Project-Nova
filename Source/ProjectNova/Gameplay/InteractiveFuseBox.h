// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractiveObject.h"
#include "InteractiveFuseBox.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FFuseBoxInteractionEvent, IInteractiveObject*);

UCLASS()
class PROJECTNOVA_API AInteractiveFuseBox : public UActorComponent, public IInteractiveObject
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractiveFuseBox();

	bool CanInteract() const override { return true; }

	FInteractionPrompt& GetInteractionPrompt() override { return Prompt; }

	void InteractionEvent(APawn* EventSender) override;

protected:
	// Called when the game starts or when spawned
	//virtual void BeginPlay() override;

	/** Whether or not the object can be interacted with. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	uint8 bCanInteract : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FInteractionPrompt Prompt;

	/** Is set to interact by default. See Edit->ProjectSettings->Input for list of action mapping names.*/
	UPROPERTY(EditAnywhere, Category = "Interaction")
	FName ActionMappingName;

	FFuseBoxInteractionEvent FuseBoxInteractionEvent;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
