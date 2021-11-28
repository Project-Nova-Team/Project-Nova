// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractiveObject.h"
#include "PushButton.generated.h"

struct FDelayedActionHandle;

UENUM()
enum EButtonState
{
	EBS_Extended,
	EBS_Retracted,
	EBS_Changing
};

UCLASS()
class UNREALPLAYGROUND_API APushButton : public AActor, public IInteractiveObject
{
	GENERATED_BODY()
	
public:	
	void InteractionEvent(APawn* EventSender) override;

	bool CanInteract() const override { return !bIsMoving; }

	FInteractionPrompt& GetInteractionPrompt() override { return Prompt; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** How far this button retracts into the panel*/
	UPROPERTY(EditAnywhere)
	float PushDepth;

	/** How long it takes for the button to retract*/
	UPROPERTY(EditAnywhere)
	float ButtonTransitionTime;

	UStaticMeshComponent* MeshComponent;

	/** Arrow pointing outward from the face of the button*/
	UStaticMesh* Mesh;

	UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "Interactable")
	FInteractionPrompt Prompt;

	/** Live state of this door*/
	TEnumAsByte<EButtonState> State;

	/** Delayed action handle for ease of LERP*/
	FDelayedActionHandle* Handle;

	void OverTimeTransition();

	uint8 bIsMoving;
};
