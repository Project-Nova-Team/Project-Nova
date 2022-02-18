#include "ShooterInventory.h"
#include "../Weapon/CombatComponent.h"
#include "../Weapon/Gun.h"

UShooterInventory::UShooterInventory()
{
	HeldAmmo.Reserve(3);
}

void UShooterInventory::BeginPlay()
{
	Super::BeginPlay();
	Combat->OnWeaponAdd.BindUObject(this, &UShooterInventory::ReceiveWeaponAdd);
}

void UShooterInventory::AddPack(const FAmmoPackage& Package)
{
	//First, see if the shooter already owns a weapon of this type
	//if it does, just add the ammo to the gun
	if (AGun* Gun = Cast<AGun>(Combat->GetWeaponOfType(Package.GunClass)))
	{
		Gun->AddAmmo(Package.AmmoAmount);
		return;
	}

	//If the shooter does not already own a gun of this type add it to the inventory
	for (FAmmoPackage& Pack : HeldAmmo)
	{
		//If we already own a package of this class type, just increase ammo
		if (Package.GunClass.Get() == Pack.GunClass.Get())
		{
			Pack.AmmoAmount += Package.AmmoAmount;
			return;
		}
	}

	//Otherwise just add a copy of the package to the inventory
	HeldAmmo.AddUnique(Package);
}

void UShooterInventory::ReceiveWeaponAdd(AWeapon* Weapon)
{
	if (AGun* AsGun = Cast<AGun>(Weapon))
	{
		//When we add a weapon, check if the inventory has a pack of ammo for that gun
		//If we do, add it to the weapon
		for (FAmmoPackage& Pack : HeldAmmo)
		{
			if (Weapon->GetClass() == Pack.GunClass.Get())
			{
				AsGun->AddAmmo(Pack.AmmoAmount);
				Pack.AmmoAmount = 0; //hacky
				return;
			}
		}
	}
}