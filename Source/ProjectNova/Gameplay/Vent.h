#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractiveObject.h"
#include "InteractableFusebox.h"
#include "Components/SplineComponent.h"
#include "Components/SplineComponent.h"
#include "Vent.generated.h"

class UStaticMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FVentEvent);

UCLASS()
class PROJECTNOVA_API AVent : public AActor, public IInteractiveObject
{
	GENERATED_BODY()
	
public:	
	AVent();

	/** Is the player currently using this vent*/
	uint8 bInUse : 1;

protected:

	/** Whether or not this vent is disabled and can be entered*/
	UPROPERTY(EditAnywhere, BlueprintSetter = SetVentDisabled, Category = "Vent")
	uint8 bDisabled : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Vent")
	USplineComponent* SplineComponent;

public:

	/// Begin Interactive Object Interface ///

	bool CanInteract() const override { return bDisabled && !bInUse; }
	FInteractionPrompt& GetInteractionPrompt() override { return Prompt; }
	void InteractionEvent(APawn* EventSender) override;

	/// End Interactive Object Interface ///

#if WITH_EDITOR
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	FORCEINLINE USplineComponent* GetSpline() { return SplineComponent; }

protected:

	/** Sets the active status of the vent. Disabled vents can be entered.*/
	UFUNCTION(BlueprintSetter)
	void SetVentDisabled(const bool bDisableVent);

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FInteractionPrompt Prompt;

	/** Set to interact by default. See Edit->ProjectSettings->Input for list of action mapping names.*/
	UPROPERTY(EditAnywhere, Category = "Interaction")
	FName ActionMappingName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* EntryFrame;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* EntryGrate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* ExitFrame;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* ExitGrate;
};
