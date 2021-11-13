#pragma once

#include "CoreMinimal.h"
#include "Trigger.h"
#include "PromptTrigger.generated.h"

UENUM()
enum EPromptSatisfier
{
	Prompt_Exit,
	Prompt_Input,
	Prompt_Duration,
	Prompt_DurationExit
};

/** Trigger that displays UI prompt on screen. Note the white list should ONLY contain pawns that are controlled by a player controller!!*/
UCLASS()
class UNREALPLAYGROUND_API APromptTrigger : public ATrigger
{
	GENERATED_BODY()

public:
	APromptTrigger();

	void SetActive(const bool Value) override;

protected:

	virtual void ExecuteTrigger(APawn* Sender) override;

	virtual void EndOverlap(AActor* OverlappedActor, AActor* OtherActor);

	/** Used For the first text in a Bark UI display. I.E. 'Press' */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prompt | UI")
	FString PrefixText;

	/** Used For the last text in a Bark UI display.I.E. 'To Crouch' */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prompt | UI")
	FString SuffixText;

	/** The texture we render to the in a Bark UI display. I.E. 'C'*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prompt | UI")
	UTexture2D* KeyTexture;

	/** 
	 * What type of action is required to complete the prompt
	 * 
	 * Prompt_Exit				Prompt closes after trigger is exited
	 * Prompt_Input				Prompt will close after InputAction input is received
	 * Prompt_Duration			Prompt will close after duration
	 * Prompt_DurationExit		Prompt will close after duration once exiting from the trigger
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prompt | Satisfier")
	TEnumAsByte<EPromptSatisfier> PromptSatisfier;

	/** Name of input action required to close this UI prompt. (See project settings)*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prompt | Satisfier")
	FName InputAction;

	/** How many seconds should this prompt last before closing*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prompt | Satisfier")
	float Duration;

private:

	/** 
	 * Pawn that is currently receiving UI prompt. This SHOULD be a collection because multiple pawns may want to receive the prompt! 
	 * This will break if more than one pawn triggers the prompt at a time
	*/
	APawn* StagedPawn;

	/** Reference to the relative binding index so we can unbind input action when necessary*/
	int32 BindingIndex;

	void ReceiveStagedInput(); 
};
