#include "ShooterHUD.h"
#include "ShooterController.h"
#include "Player/Shooter.h"
#include "Weapon/CombatComponent.h"

AShooterHUD::AShooterHUD()
{
	PrimaryActorTick.bCanEverTick = true;
	bShowHUD = false;
}

void AShooterHUD::Initialize()
{
	Combat = Cast<AShooter>(PlayerOwner->GetPawn())->GetCombat();
}

void AShooterHUD::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	bPlayerHasWeapon = Combat->GetPrimaryWeapon() != nullptr;
	Bloom = Combat->GetCurrentBloom();

	if (bPlayerHasWeapon)
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
}