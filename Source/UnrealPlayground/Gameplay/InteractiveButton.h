// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractiveObject.h"
#include "InteractiveButton.generated.h"

struct FDelayedActionHandle;

UENUM()
enum EButtonState
{
	EBS_Extended,
	EBS_Retracted,
	EBS_Changing
};

UCLASS()
class UNREALPLAYGROUND_API AInteractiveButton : public AActor, public IInteractiveObject
{
	GENERATED_BODY()

public:

	AInteractiveButton();

	void InteractionEvent(APawn* EventSender) override;

	bool CanInteract() const override { return !bIsLocked && !bIsMoving; }

	FInteractionPrompt& GetInteractionPrompt() override { return Prompt; }

	/** Sets the lock status of this Button*/
	UFUNCTION(BlueprintSetter)
	void SetIsLocked(const bool Value);

	/** Should this button be opening*/
	FORCEINLINE bool ShouldRetract() const { return !bIsLocked && State == EBS_Extended; }

	/** Should this Button be shutting.*/
	FORCEINLINE bool ShouldExtend() const { return !bIsLocked && State == EBS_Retracted; }

protected:

	void MaybeChangeButtonState();

	/** Button Retracts into the panel*/
	void Retract();

	/** Button Extends outward from the panel*/
	void Extend();

	/** How far this button retracts into the panel*/
	UPROPERTY(EditAnywhere)
		float PushDepth;

	/** How long it takes for the button to retract*/
	UPROPERTY(EditAnywhere)
		float ButtonTransitionTime;

	/** Whether or not the button is currently locked*/
	UPROPERTY(EditAnywhere, BlueprintSetter = SetIsLocked, Category = "Door")
		uint8 bIsLocked : 1;

	/**This is mesh of button, not panel*/
	UPROPERTY(BlueprintReadWrite)
	UStaticMeshComponent* Mesh;

	UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "Interactable")
	FInteractionPrompt Prompt;

	/** Live state of this door*/
	TEnumAsByte<EButtonState> State;

	/** Delayed action handle for ease of LERP*/
	FDelayedActionHandle* Handle;

	/** Move Button*/
	void OverTimeTransition(const EButtonState TargetState);

	uint8 bIsMoving;


};

