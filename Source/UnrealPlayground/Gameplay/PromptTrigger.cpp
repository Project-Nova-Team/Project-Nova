#include "PromptTrigger.h"
#include "../ShooterGameMode.h"
#include "../Utility/DelayedActionManager.h"
#include "Components/BoxComponent.h"

APromptTrigger::APromptTrigger()
{
	PrefixText = "Press ";
	SuffixText = " to Crouch";
	InputAction = TEXT("Crouch");
}

void APromptTrigger::EndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	//The object that exited the trigger was capable of triggering it in the firstplace
	if (TriggerWhiteList.Contains(OtherActor->GetClass()))
	{
		OnTriggerExited.Broadcast(Cast<APawn>(OtherActor));

		//We exited the trigger marked as prompt. Hide the UI widget but don't disable the trigger
		if (PromptSatisfier == Prompt_Input && StagedPawn != nullptr)
		{
			StagedPawn->InputComponent->RemoveActionBinding(BindingIndex);
		}

		//We exited the trigger in exit, just turn off
		else if (PromptSatisfier == Prompt_Exit && !bTriggerMoreThanOnce)
		{
			SetActive(false);
		}
	}
}

void APromptTrigger::ExecuteTrigger(APawn* Sender)
{
	OnTriggerActivated.Broadcast(Sender);

	if (PromptSatisfier == Prompt_Duration)
	{
		//If we can trigger more than once we dont want to disable the trigger, just hide the UI
		if (bTriggerMoreThanOnce)
		{
			//Have to upcast since the delegate doesn't like imperfect types
			GetWorld()->GetAuthGameMode<AShooterGameMode>()->GetDelayedActionManager()->
				StartDelayedAction(this, &APromptTrigger::EndOverlap, Duration, Cast<AActor>(this), Cast<AActor>(Sender));
		}

		//Otherwise we just disable the trigger after the duration (which will also hide the UI)
		else
		{
			GetWorld()->GetAuthGameMode<AShooterGameMode>()->GetDelayedActionManager()->
				StartDelayedAction(this, &APromptTrigger::SetActive, Duration, false);
		}	
	}

	else if (PromptSatisfier == Prompt_Input)
	{
		//Bind specified action to a function that will disable the UI prompt once the action is executed
		StagedPawn = Sender;
		BindingIndex = StagedPawn->InputComponent->BindAction(InputAction, IE_Pressed, this, &APromptTrigger::ReceiveStagedInput).GetHandle();
	}
}

void APromptTrigger::ReceiveStagedInput()
{
	if (!bTriggerMoreThanOnce)
	{
		SetActive(false);
	}
}

void APromptTrigger::SetActive(const bool Value)
{
	if (bIsActive != Value)
	{
		bIsActive = Value;

		if (bIsActive)
		{
			TriggerVolume->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		}

		else
		{
			TriggerVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			if (PromptSatisfier == Prompt_Input && StagedPawn != nullptr) 
			{
				StagedPawn->InputComponent->RemoveActionBinding(BindingIndex);
			}
		}
	}
}