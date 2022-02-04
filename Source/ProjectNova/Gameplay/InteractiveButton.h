// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractiveObject.h"
#include "../ShooterGameMode.h"
#include "Components/ArrowComponent.h"
#include "../Utility/DelayedActionManager.h"
#include "InteractiveButton.generated.h"

struct FDelayedActionHandle;

class UAudioComponent;
class UMaterialInstanceDynamic;

UENUM()
enum EButtonState
{
	EBS_Extended,
	EBS_Retracted,
	EBS_Changing
};

UCLASS()
class PROJECTNOVA_API AInteractiveButton : public AActor, public IInteractiveObject
{
	GENERATED_BODY()

public:

	AInteractiveButton();

	void InteractionEvent(APawn* EventSender) override;

	bool CanInteract() const override { return !bIsLocked && !bIsMoving; }

	const FInteractionPrompt& GetInteractionPrompt() const override { return Prompt; }

	/** Sets the lock status of this Button*/
	UFUNCTION(BlueprintSetter)
	void SetIsLocked(const bool Value);

	/** Should this button be opening*/
	FORCEINLINE bool ShouldRetract() const { return !bIsLocked && State == EBS_Extended && bIsInteracted; }

	/** Should this Button be shutting.*/
	FORCEINLINE bool ShouldExtend() const { return !bIsLocked && State == EBS_Retracted; }

	uint8 bIsInteracted : 1;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* Panel;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* Button;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
		UArrowComponent* Arrow;

	UMaterialInstanceDynamic* EmissiveMaterial;

	/** Component that plays the sound*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sound")
	UAudioComponent* AudioComponent;

	/**
	* Invoked when SetIsLocked changes lock status (providing the new lock state)
	* This lets us tell blueprint subclass to update an emissive material
	*/
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Lock Changed"))
	void ReceiveLockStatusChange(const bool LockStatus);

	void SetIsLockedImpl(const bool Value);

	void MaybeChangeButtonState();

	void BeginPlay();

	void StartDelayedAction(EButtonState TargetState);

	/** Initial offset of the button mesh to the panel*/
	FVector InitialOffset;

	/** How far this button retracts into the panel*/
	UPROPERTY(EditAnywhere)
		float PushDepth;

	/** How long it takes for the button to retract*/
	UPROPERTY(EditAnywhere)
		float ButtonTransitionTime;

	/** Whether or not the button is currently locked*/
	UPROPERTY(EditAnywhere, BlueprintSetter = SetIsLocked, Category = "Button")
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

	uint8 bIsMoving : 1;
};