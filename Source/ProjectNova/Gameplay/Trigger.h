#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Trigger.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTriggerEvent, APawn*, Sender);

class UBoxComponent;
class UBillboardComponent;

UCLASS(HideCategories=("Collision", "Physics", "Navigation", "Rendering", "Tags", "Cooking", "Replication", "Input", "HLOD", "Mobile", "Asset User Data"))
class PROJECTNOVA_API ATrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	ATrigger();

	UFUNCTION(BlueprintSetter)
	virtual void SetActive(const bool Value);

	FTriggerEvent OnTriggerActivated;

	FTriggerEvent OnTriggerExited;

#if WITH_EDITOR
	void PostEditChangeProperty(FPropertyChangedEvent& Event) override;
#endif

protected:

	/** When enabled, this trigger will execute its logic when a valid actor enters it*/
	UPROPERTY(EditAnywhere, BlueprintSetter = SetActive, Category = "Trigger | General")
	uint8 bIsActive : 1;

	/** If true, this trigger will activate everytime it is entered*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger | General")
	uint8 bTriggerMoreThanOnce : 1;

	/** This trigger will activate when Engi enters it while it is enabled*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger | Activation")
	uint8 bActivatedByEngi: 1;

	/** This trigger will activate when Gorms or the Big Guy enter it while it is enabled*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger | Activation")
	uint8 bActivatedByAI : 1;

	/** Collision volume that acts as the trigger region*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collider")
	UBoxComponent* TriggerVolume;

	UFUNCTION(BlueprintImplementableEvent)
	void TriggerActivated(APawn* Activator);

	UFUNCTION(BlueprintImplementableEvent)
	void TriggerExited(APawn* Activator);

	bool IsValidActivator(AActor* Activator) const;

	virtual void SetActiveInternal();

#if WITH_EDITORONLY_DATA
	/** Simple editor sprite to help designers see trigger locations*/
	UPROPERTY()
	UBillboardComponent* SpriteComponent;
#endif

	/** Called when the trigger is activated*/
	virtual void ExecuteTrigger(APawn* Sender);

	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void BeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	virtual void EndOverlap(AActor* OverlappedActor, AActor* OtherActor);
};
