#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "QuickTimeManager.generated.h"

USTRUCT(BlueprintType)
struct FQuickTimeInput
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName ActionName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* IdleTexture;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* ActiveTexture;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FQuickTimeEventCompletion);

/**
 * This class is responsible for handling quick time events that occur
 * When Engi fights off an AI enemy (usually a gorm)
 **/
UCLASS()
class PROJECTNOVA_API AQuickTimeManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AQuickTimeManager();

protected:

	/** If true, a quick time action is currently active*/
	UPROPERTY(BlueprintReadOnly)
	uint8 bActive : 1;

	/** Number of seconds a quick time action takes before returning the result*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quick Time")
	float ActionTime;

	/** Number of seconds between flicker of the UI widget showing the necessary key to press*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quick Time")
	float WidgetToggleTime;

	/** Number of presses required for a success during a quick time*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quick Time")
	int32 InputsRequired;

	/** Collection of InputActions and their respecitve on screen mash textures*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quick Time")
	TArray<FQuickTimeInput> QuickTimeInputs;

	UPROPERTY(BlueprintAssignable)
	FQuickTimeEventCompletion OnQuickTimeComplete;

private:

	/** Binding handle for requested input event*/
	int32 BindingHandle;

	/** Number of actions completed in this quick time event*/
	int32 ActionCount;

	/** Number of actions required to complete this quick time event*/
	int32 RequiredActions;

	/** Number of inputs received during a particular action*/
	int32 InputCount;

	/** Selected InputAction*/
	int32 ActiveIndex;

	/** True if an action has been completed but is awaiting the next action in the event*/
	uint8 bAwaitingNextAction : 1;

	/** Flag for displayed texture state*/
	uint8 bTextureActive : 1;

	/** Number of success the AI has had in this event*/
	int32 AISuccesses;

	/** Number of success the shooter has had in this event*/
	int32 ShooterSuccesses;

	/** Handle responsible for ending an action after ActionTime*/
	FTimerHandle ActionHandle;

	/** Handle responsible for flipinng between active and inactive texture*/
	FTimerHandle WidgetHandle;


	class AShooter* Shooter;

	class ABaseAI* AI;

	/** HUD element responsible for displaying UI information*/
	class UQuickTimeWidget* QuickTimeWidget;

public:	

	/** 
	 * Initiates the quick time sequence with the provided number of necessary actions needed to complete
	 *
	 * @param	InstigatingAI	The AI actor involved in this quick time
	 * @param	EventCount		Number of quick time actions required to complete this event
	 * 
	 * @returns	True if a quicktime event was successfully started
	 */
	UFUNCTION(BlueprintCallable)
	bool StartQuickTime(class ABaseAI* InstigatingAI, int32 EventCount = 1);

	/** If a quick time event is active, Calling this will display actually begin the next */
	UFUNCTION(BlueprintCallable)
	void StartNextAction();

	/** Called to clear the quick time event and allow new ones to occur*/
	UFUNCTION(BlueprintCallable)
	void FinishQuickTimeEvent();

	void Init();

protected:

	void DetermineActionResult();

	void ToggleTexture();

	void ReceiveInput(); 
};
