#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "QuickTimeAction.h"
#include "QuickTimeManager.generated.h"

/**
 * This class is responsible for handling quick time events that occur
 * When Engi fights off an AI enemy (usually a gorm)
 **/
UCLASS()
class PROJECTNOVA_API AQuickTimeManager : public AActor
{
	GENERATED_BODY()

protected:

	/** If true, a quick time action is currently active*/
	UPROPERTY(BlueprintReadOnly)
	uint8 bActive : 1;

	/** Number of seconds between flicker of the UI widget showing the necessary key to press*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quick Time")
	float WidgetToggleTime;

private:

	/** Binding handle for requested input event*/
	int32 BindingHandle;

	/** Number of inputs received during the current action*/
	int32 InputCount;

	/** Index of the current QT action running*/
	int32 ActionIndex;

	/** Action currently being played*/
	UQuickTimeAction* CurrentAction;

	/** Flag for displayed texture state*/
	uint8 bTextureActive : 1;

	/** Result of the current action, fail or success if the player entered the right input*/
	uint8 bCurrentActionResult : 1;

	/** Handle responsible for ending an action after ActionTime*/
	FTimerHandle ActionHandle;

	/** Handle responsible for flipinng between active and inactive texture*/
	FTimerHandle WidgetHandle;

	class AShooter* Shooter;

	class ABaseAI* AI;

	class AShooterCutscene* Cutscene;

	/** HUD element responsible for displaying UI information*/
	class UQuickTimeWidget* QuickTimeWidget;

public:	

	/** Initializes necessary services*/
	void Init();

	/** 
	 * Initiates a quick time sequence
	 *
	 * @param	InstigatingAI	The AI actor involved in this quick time
	 * 
	 * @returns	True if a quicktime event was successfully started
	 */
	UFUNCTION(BlueprintCallable)
	bool StartQuickTime(class ABaseAI* InstigatingAI);

private:

	/** Creates a transform that places the cutscene in front of the AI and looking at them*/
	FTransform ComputeQuickTimeLocation();

	/** Starts an action playing it to completion*/
	void StartAction();

	/** Upon completing a struggle, determine the result*/
	void CompleteStruggle();

	/** Called upon completion of an action. Starts a new action if one exists*/
	void CompleteResult();

	/** Increments input count when receiving proper QT input*/
	void ReceiveInput();

	/** Swaps texture on the quick time widget*/
	void ToggleTexture();
};
