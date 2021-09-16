#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ShooterHUD.generated.h"

class AShooter;
class UShooterCombatComponent;

UCLASS()
class UNREALPLAYGROUND_API AShooterHUD : public AHUD
{
	GENERATED_BODY()

public:
	AShooterHUD();
	virtual void Tick(float DeltaTime) override;
	void Initialize();

protected:
	/** Max ammo that can be stored in this weapon after reloading*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
		int MaxAmmoInWeapon;

	/** Current amount of ammo ready to be fired*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
		int AmmoInWeapon;

	/** The amount of extra ammo the player has*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
		int ExcessAmmo;

	/** The amount of bloom currently attached to the combat component*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
		float Bloom;

	UPROPERTY(BlueprintReadOnly)
		uint8 bPlayerHasWeapon : 1;

private:

	/** Combat component attached to player*/
	UShooterCombatComponent* Combat;
};
