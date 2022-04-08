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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Spline")
	USplineComponent* SplineComponent;

	bool CanInteract() const override { return bIsDisabled && OverlappedPawns > 0; }

	FInteractionPrompt& GetInteractionPrompt() override { return Prompt; }

	void InteractionEvent(APawn* EventSender) override;

	USplineComponent* GetSpline() { return Spline; }

	float GetCrawlSpeed() { return CrawlSpeed; }

	float GetLerpToStandingSpeed() { return LerpToStandingSpeed; }

	float GetLerpToCrawlSpeed() { return LerpToCrawlSpeed; }

	/** These bools are used for ventstate to determine which side of the spline to start from */
	uint8 bIsOverlappingLeftTrigger : 1;

	uint8 bIsOverlappingRightTrigger : 1;

	/** Turns the grate back on*/
	void ReEnableGrate();

	void PostInitializeComponents() override;

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

	/** How fast the player lerps from standing to crawl position. 2.f is about right */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed")
	float LerpToCrawlSpeed;

	/** How fast the player lerps from crawl to standing position. 2.f is about right */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed")
	float LerpToStandingSpeed;
	
	/** How fast the player moves through the vent. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed")
	float CrawlSpeed;

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

	/** Interactive Fusebox*/
	UPROPERTY(Category = Health, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UChildActorComponent* Fusebox;

	/** Disables the grate for DisableDuration*/
	UFUNCTION()
	void DisableGrateForDuration();

	/** Calls above function if conditions are met*/
	void MaybeReEnableGrate();

	UFUNCTION()
	void ComponentBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void ComponentEndOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void ReceiveFuseboxFixed(APawn* EventSender);
	
	UPROPERTY(BlueprintAssignable)
	FVentEvent OnVentEnabled;

	UPROPERTY(BlueprintAssignable)
	FVentEvent OnVentDisabled;

private:

	/** Need this in the event a pawn is blocking the vent when we are trying to enable the lazers (which would get the object stuck)*/
	FORCEINLINE bool ShouldEnable() { return OverlappedPawns == 0 && bIsDisabled && !bDelayRunning; }

	AInteractableFusebox* FuseboxRef;

	/** See grate trigger*/
	int32 OverlappedPawns;

	//these bools should be an enum state but i am lazy

	/** Is the grate currently disabled*/
	uint8 bIsDisabled : 1;

	/** Is the delayed action running*/
	uint8 bDelayRunning : 1;
};
