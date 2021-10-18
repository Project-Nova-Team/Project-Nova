#include "ShooterHUD.h"
#include "ShooterController.h"
#include "Player/Shooter.h"
#include "../UnrealPlayground/Weapon/Gun.h"
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

	if(HeldAsGun != nullptr)
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