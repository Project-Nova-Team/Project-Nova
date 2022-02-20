#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractiveObject.h"
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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Spline")
	USplineComponent* SplineComponent;

	bool CanInteract() const override { return bCanInteract; }

	FInteractionPrompt& GetInteractionPrompt() override { return Prompt; }

	FName& GetInteractionMappingName() override { return ActionMappingName; }

	void InteractionEvent(APawn* EventSender) override;

	void RecieveLookedAt(APawn* EventSender);

	void RecieveLookedAway(APawn* EventSender, int32 MappingIndexToRemove) override;

	USplineComponent* GetSpline() { return Spline; }

protected:
	virtual void BeginPlay() override;

	/** Whether or not the object can be interacted with. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	uint8 bCanInteract : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FInteractionPrompt Prompt;

	/** Is set to interact by default. See Edit->ProjectSettings->Input for list of action mapping names.*/
	UPROPERTY(EditAnywhere, Category = "Interaction")
	FName ActionMappingName;

	/** How many seconds after taking damage do the lazers stay disabled for*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vent")
	float DisableDuration;

	UPROPERTY(Category = Spline, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USplineComponent* Spline;

	UPROPERTY(Category = Mesh, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* LeftFrame;

	UPROPERTY(Category = Mesh, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* RightFrame;

	UPROPERTY(Category = Mesh, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* LeftGrate;

	UPROPERTY(Category = Mesh, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* RightGrate;

	/** Utilize overlap so we never close unless all objects have exited*/
	UPROPERTY(Category = Mesh, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* LeftGrateTrigger;

	/** Utilize overlap so we never close unless all objects have exited*/
	UPROPERTY(Category = Mesh, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* RightGrateTrigger;

	UPROPERTY(Category = Health, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UHealthComponent* Health;

	/** Disables the grate for DisableDuration*/
	UFUNCTION()
	void DisableGrateForDuration();

	/** Turns the grate back on*/
	void ReEnableGrate();

	/** Calls above function if conditions are met*/
	void MaybeReEnableGrate();

	UFUNCTION()
	void ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UPROPERTY(BlueprintAssignable)
	FVentEvent OnVentEnabled;

	UPROPERTY(BlueprintAssignable)
	FVentEvent OnVentDisabled;

private:

	/** Need this in the event a pawn is blocking the vent when we are trying to enable the lazers (which would get the object stuck)*/
	FORCEINLINE bool ShouldEnable() { return OverlappedPawns == 0 && bIsDisabled && !bDelayRunning; }

	/** See grate trigger*/
	int32 OverlappedPawns;

	//these bools should be an enum state but i am lazy

	/** Is the grate currently disabled*/
	uint8 bIsDisabled : 1;

	/** Is the delayed action running*/
	uint8 bDelayRunning : 1;
};
