#include "EngineerHUD.h"
#include "Engineer.h"
#include "EngineerController.h"
#include "Blueprint/UserWidget.h"


void AEngineerHUD::Initialize()
{
	Engineer = Cast<AEngineer>(PlayerOwner->GetPawn());

	if (Engineer != nullptr)
	{
		BuildBlueprintWidgets();

		//Get HUD update events for interaction prompts
		Engineer->OnInteractionUpdate.BindUObject(this, &AEngineerHUD::ReceiveInteractionUpdate);

		// Fire the blueprint event when the combat component informs us we should update
		Combat = Engineer->GetCombatComponent();	
		Combat->OnUpdateHUD.BindUObject(this, &AEngineerHUD::ReceiveWeaponUpdate);	
	}
}

void AEngineerHUD::SetPauseDisplay(const bool bShowPauseMenu)
{
	if (bShowPauseMenu)
	{
		PauseMenuWidget->SetVisibility(ESlateVisibility::Visible);
		PlayerOwner->bShowMouseCursor = true;
	}

	else
	{
		PlayerOwner->bShowMouseCursor = false;

		ExitConfirmationWidget->SetVisibility(ESlateVisibility::Collapsed);
		PauseMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
		SettingsMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
		ControlsMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void AEngineerHUD::ReceiveInteractionUpdate(IInteractiveObject* Info)
{
	if (Info == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("Revoke"));
		InteractionPromptRevoke();
	}

	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Provide"));
		InteractionPromptProvided(Info->GetInteractionPrompt());
	}
}
