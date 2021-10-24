#include "ShooterHUD.h"
#include "ShooterController.h"
#include "Player/Shooter.h"
#include "../UnrealPlayground/Weapon/Gun.h"
<<<<<<< Updated upstream
#include "Weapon/ShooterCombatComponent.h"
=======
#include "../UnrealPlayground/ShooterGameMode.h"
#include "Blueprint/UserWidget.h"
#include "Weapon/CombatComponent.h"
>>>>>>> Stashed changes

AShooterHUD::AShooterHUD()
{
	PrimaryActorTick.bCanEverTick = true;
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
<<<<<<< Updated upstream
=======

	Combat->OnArsenalAddition.AddUObject(this, &AShooterHUD::ReceiveWeapon);
	Combat->OnArsenalRemoval.AddUObject(this, &AShooterHUD::ReleaseWeapon);

	if (GetWorld()->GetAuthGameMode<AShooterGameMode>())
		GetWorld()->GetAuthGameMode<AShooterGameMode>()->OnPause.AddUObject(this, &AShooterHUD::ShowPauseMenu);

	Shooter->OnScanHit.AddDynamic(this, &AShooterHUD::ShowInteractionPrompt);
	Shooter->OnScanMiss.AddDynamic(this, &AShooterHUD::HideInteractionPrompt);
>>>>>>> Stashed changes
}

void AShooterHUD::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Combat == nullptr)
	{
		return;
	}

	bPlayerHasWeapon = Combat->GetPrimaryWeapon() != nullptr;
	Bloom = Combat->GetWeaponBloom();

<<<<<<< Updated upstream
	if (bPlayerHasWeapon)
=======
	AGun* HeldAsGun = Cast<AGun>(Combat->GetHeldWeapon());

	if (HeldAsGun != nullptr)
>>>>>>> Stashed changes
	{
		const FWeaponUIData Data = Combat->GetPrimaryWeapon()->GetWeaponUI();
		MaxAmmoInWeapon = Data.ClipSize;
		AmmoInWeapon = Data.AmmoInClip;
		ExcessAmmo = Data.ExcessAmmo;
	}

	else
	{
		MaxAmmoInWeapon = 0;
		AmmoInWeapon = 0;
		ExcessAmmo = 0;
	}
<<<<<<< Updated upstream
}
=======

	OnUpdate.Broadcast();
}

void AShooterHUD::ShowPauseMenu()
{
	if (!bIsPaused)
	{
		PauseMenuWidget->SetVisibility(ESlateVisibility::Visible);

		if (Shooter->GetController<APlayerController>())
		{
			Shooter->GetController<APlayerController>()->SetPause(true);
			Shooter->GetController<APlayerController>()->bShowMouseCursor = true;
		}

		bIsPaused = true;
	}
	else
	{
		HidePauseMenu();
		bIsPaused = false;
	}
}

void AShooterHUD::HidePauseMenu()
{
	if (Shooter->GetController<APlayerController>())
	{
		Shooter->GetController<APlayerController>()->bShowMouseCursor = false;
		Shooter->GetController<APlayerController>()->SetPause(false);
	}
	PauseMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
}

void AShooterHUD::ShowInteractionPrompt(FHitResult Hit)
{
	if (Hit.GetActor()->IsA(AVaultObject::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Hit.GetActor()->GetName());
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
>>>>>>> Stashed changes
