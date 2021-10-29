#include "ShooterHUD.h"
#include "ShooterController.h"
#include "Player/Shooter.h"
#include "../UnrealPlayground/Weapon/Gun.h"
#include "../UnrealPlayground/ShooterGameMode.h"
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

	Combat->OnArsenalAddition.AddUObject(this, &AShooterHUD::ReceiveWeapon);
	Combat->OnArsenalRemoval.AddUObject(this, &AShooterHUD::ReleaseWeapon);

	if (GetWorld()->GetAuthGameMode<AShooterGameMode>())
		GetWorld()->GetAuthGameMode<AShooterGameMode>()->OnPause.AddUObject(this, &AShooterHUD::ShowPauseMenu);

	Shooter->OnScanHit.AddDynamic(this, &AShooterHUD::ShowInteractionPrompt);
	Shooter->OnScanMiss.AddDynamic(this, &AShooterHUD::HideInteractionPrompt);
}

void AShooterHUD::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Bloom = Combat->GetWeaponBloom();
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
	bPlayerHasWeapon = Combat->GetHeldWeapon() != nullptr;
	Bloom = Combat->GetWeaponBloom();

	AGun* HeldAsGun = Cast<AGun>(Combat->GetHeldWeapon());

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

void AShooterHUD::ShowInteractionPrompt(FHitResult Hit)
{
	if (Hit.GetActor()->IsA(AVaultObject::StaticClass()))
	{
		Shooter->bIsLookingAtVaultObject = true;
		if (Shooter->GetCanVault())
		{
			VaultPromptWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
			Shooter->GetShooterMovement()->bCanVault = true;
		}
	}
	else
	{
		InteractionPromptWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void AShooterHUD::HideInteractionPrompt(FHitResult Hit)
{
	Shooter->bIsLookingAtVaultObject = false;
	Shooter->GetShooterMovement()->bCanVault = false;
	InteractionPromptWidget->SetVisibility(ESlateVisibility::Collapsed);
	VaultPromptWidget->SetVisibility(ESlateVisibility::Collapsed);
}