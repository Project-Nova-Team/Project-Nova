#include "ShooterHUD.h"
#include "ShooterController.h"
#include "Player/Shooter.h"
#include "Gameplay/HealthComponent.h"
#include "Blueprint/UserWidget.h"


void AShooterHUD::Initialize()
{
	Shooter = Cast<AShooter>(PlayerOwner->GetPawn());

	if (Shooter != nullptr)
	{
		BuildBlueprintWidgets();

		//Get HUD update events for interaction prompts
		Shooter->OnInteractionUpdate.BindUObject(this, &AShooterHUD::ReceiveInteractionUpdate);
		Shooter->GetHealth()->OnDeath.AddDynamic(this, &AShooterHUD::ShowDeathDisplay);

		// Fire the blueprint event when the combat component informs us we should update
		Combat = Shooter->GetCombat();
		Combat->OnUpdateHUD.BindUObject(this, &AShooterHUD::ReceiveWeaponUpdate);
	}
}

void AShooterHUD::SetPauseDisplay(const bool bShowPauseMenu)
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

void AShooterHUD::ShowDeathDisplay()
{
	PlayerOwner->bShowMouseCursor = true;

	//Hide everything else
	InteractionPromptWidget->SetVisibility(ESlateVisibility::Collapsed);
	WeaponDisplayWidget->SetVisibility(ESlateVisibility::Collapsed);
	ExitConfirmationWidget->SetVisibility(ESlateVisibility::Collapsed);
	PauseMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
	//SettingsMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
	//ControlsMenuWidget->SetVisibility(ESlateVisibility::Collapsed);

	//Show the death screen
	DeathScreenWidget->SetVisibility(ESlateVisibility::Visible);
}

void AShooterHUD::ReceiveInteractionUpdate(IInteractiveObject* Info)
{
	if (Info == nullptr)
	{
		InteractionPromptRevoke();
	}

	else
	{
		InteractionPromptProvided(Info->GetInteractionPrompt());
	}
}