#include "ShooterHUD.h"
#include "ShooterController.h"
#include "Player/Shooter.h"
#include "./Weapon/Gun.h"
#include "Gameplay/GeneratorPiece.h"
#include "ShooterGameMode.h"
#include "Blueprint/UserWidget.h"
#include "Weapon/CombatComponent.h"

AShooterHUD::AShooterHUD()
{
	bShowHUD = false;
}

void AShooterHUD::Initialize()
{
	APawn* Pawn = PlayerOwner->GetPawn();

	if (Pawn->IsA(AShooter::StaticClass()))
	{
		Shooter = Cast<AShooter>(Pawn);
		Combat = Shooter->GetCombat();
	}

	//Temp fix. Causes crashes in levels where the player is not auto-possesed because the HUD auto-posses the default pawn instead
	if (Combat != nullptr)
	{
		Combat->OnArsenalAddition.AddUObject(this, &AShooterHUD::ReceiveWeapon);
		Combat->OnArsenalRemoval.AddUObject(this, &AShooterHUD::ReleaseWeapon);

		if (GetWorld()->GetAuthGameMode<AShooterGameMode>())
			GetWorld()->GetAuthGameMode<AShooterGameMode>()->OnPause.AddUObject(this, &AShooterHUD::ShowPauseMenu);

		Shooter->OnScanHit.AddDynamic(this, &AShooterHUD::ShowInteractionPrompt);
		Shooter->OnScanMiss.AddDynamic(this, &AShooterHUD::HideInteractionPrompt);
	}	
}

void AShooterHUD::Tick(float DeltaTime)
{
	if (Combat != nullptr)
	{
		Super::Tick(DeltaTime);
		Bloom = Combat->GetWeaponBloom();
	}
}

void AShooterHUD::ReceiveWeapon(AWeapon* NewWeapon)
{
	NewWeapon->OnUpdateUI.BindUObject(this, &AShooterHUD::InternalUpdate);
	InternalUpdate();
}

void AShooterHUD::ReleaseWeapon(AWeapon* NewWeapon)
{
	NewWeapon->OnUpdateUI.Unbind();
	InternalUpdate();
}

void AShooterHUD::InternalUpdate()
{
	Bloom = Combat->GetWeaponBloom();

	AGun* HeldAsGun = Cast<AGun>(Combat->GetHeldWeapon());
	bPlayerHasGun = HeldAsGun != nullptr;

	if (HeldAsGun != nullptr)
	{
		const FGunUIData Data = HeldAsGun->GetGunUI();
		MaxAmmoInWeapon = Data.ClipSize;
		AmmoInWeapon = Data.AmmoInClip;
		ExcessAmmo = Data.ExcessAmmo;

		//TODO set FWeaponUIData fields here as well
	}

	else
	{
		MaxAmmoInWeapon = 0;
		AmmoInWeapon = 0;
		ExcessAmmo = 0;
	}

	OnUpdate.Broadcast();
}


void AShooterHUD::ShowPauseMenu()
{
	if (!bIsPaused)
	{
		if (PauseMenuWidget->Visibility != ESlateVisibility::Visible)
		{
			PauseMenuWidget->SetVisibility(ESlateVisibility::Visible);

			if (Shooter->GetController<APlayerController>())
			{
				Shooter->GetController<APlayerController>()->SetPause(true);
				Shooter->GetController<APlayerController>()->bShowMouseCursor = true;
			}

			bIsPaused = true;
		}
	}
	else
	{
		HideUI();
		bIsPaused = false;
	}
}

void AShooterHUD::ShowDeathScreen()
{
	DeathScreenWidget->SetVisibility(ESlateVisibility::Visible);
	Shooter->GetController<APlayerController>()->SetPause(true);
	Shooter->GetController<APlayerController>()->bShowMouseCursor = true;
}

void AShooterHUD::HideUI()
{
	if (Shooter->GetController<APlayerController>())
	{
		Shooter->GetController<APlayerController>()->bShowMouseCursor = false;
		Shooter->GetController<APlayerController>()->SetPause(false);
	}

	// eventually add method that checks for any open widgets and closes them
	ExitConfirmationWidget->SetVisibility(ESlateVisibility::Collapsed);
	PauseMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
	SettingsMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
	ControlsMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
}

void AShooterHUD::ShowInteractionPrompt(const FInteractionPrompt& Hit)
{
	if (!bInteractionPromptActive)
	{
		bInteractionPromptActive = true;
		InteractionPromptWidget->SetVisibility(ESlateVisibility::HitTestInvisible);		
	}

	//PUTTING THIS HERE BECAUSE ITS GETTING DELETED
	if (true) // Hit != LastScan
	{
		LastScan = Hit;
		OnInteractionPromptChange.Broadcast();	
	}
}

void AShooterHUD::HideInteractionPrompt(const FInteractionPrompt& Hit)
{
	if (bInteractionPromptActive)
	{
		bInteractionPromptActive = false;
		InteractionPromptWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}