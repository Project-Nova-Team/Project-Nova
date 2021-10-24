#include "ShooterHUD.h"
#include "ShooterController.h"
#include "Player/Shooter.h"
#include "../UnrealPlayground/Weapon/Gun.h"
#include "Weapon/ShooterCombatComponent.h"

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