#pragma once

#include "CoreMinimal.h"
#include "Trigger.h"
#include "InteractiveObject.h"
#include "PromptTrigger.generated.h"

UENUM()
enum EPromptSatisfier
{
	Prompt_Exit,
	Prompt_Input,
	Prompt_Duration
};

/** Trigger that displays UI prompt on screen. Note the white list should ONLY contain pawns that are controlled by a player controller!!*/
UCLASS(HideCategories=("Trigger | Activation"))
class PROJECTNOVA_API APromptTrigger : public ATrigger
{
	GENERATED_BODY()

public:
	APromptTrigger();

protected:

	virtual void ExecuteTrigger(APawn* Sender) override;

	virtual void EndOverlap(AActor* OverlappedActor, AActor* OtherActor);

	void SetActiveInternal() override;


	/** 
	 * What type of action is required to complete the prompt
	 * 
	 * Prompt_Exit				Prompt closes after trigger is exited
	 * Prompt_Input				Prompt will close after InputAction input is received
	 * Prompt_Duration			Prompt will close after duration
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger | Prompt")
	TEnumAsByte<EPromptSatisfier> PromptSatisfier;

	/** Name of input action required to close this UI prompt. (See project settings)*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger | Prompt")
	FName InputAction;

	/** Seconds prompt lasts before closing*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger | Prompt")
	float Duration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger | Prompt")
	FInteractionPrompt Prompt;

	/**
	 * Pawn that is currently receiving UI prompt. This SHOULD be a collection because multiple pawns may want to receive the prompt!
	 * This will break if more than one pawn triggers the prompt at a time
	*/
	UPROPERTY(BlueprintReadOnly)
	APawn* StagedPawn;

private:

	/** Reference to the relative binding index so we can unbind input action when necessary*/
	int32 BindingIndex;

	void ReceiveStagedInput(); 
};
