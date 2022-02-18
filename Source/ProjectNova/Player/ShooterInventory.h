#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Gameplay/AmmoPickup.h"
#include "ShooterInventory.generated.h"

class AShooter;
class UCombatComponent;
class AWeapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTNOVA_API UShooterInventory : public UActorComponent
{
	GENERATED_BODY()

public:	
	UShooterInventory();

	AShooter* Shooter;
	UCombatComponent* Combat;

	void AddPack(const FAmmoPackage& Package);

protected:

	void BeginPlay() override;

	/** Invoked by CombatComponent OnWeaponAdd*/
	void ReceiveWeaponAdd(AWeapon* Weapon);
	
	/** Collection of ammo */
	TArray<FAmmoPackage> HeldAmmo;
};
