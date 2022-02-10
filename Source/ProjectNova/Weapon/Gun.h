#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "Gun.generated.h"

UCLASS()
class PROJECTNOVA_API AGun : public AWeapon
{
	GENERATED_BODY()

public:
	AGun();

	/** Socket on the gun's mesh used to determine firing location*/
	static FName BarrelSocketName;

	/** Current amount of recoil active on the weapon. This is the angular momentum applied by recoil*/
	float ActiveRecoil;

	/** Maximum amount of angular offset caused by recoil*/
	UPROPERTY(EditAnywhere, Category = "Weapon | Recoil")
	float RecoilAngularLimit;

	/** Controls how quickly the camera position resets after recoil finishes decaying*/
	UPROPERTY(EditAnywhere, Category = "Weapon | Recoil")
	float RecoilRecovery;

protected:

	/** Amount of damage dealt by each bullet that hits an enemy*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Damage")
	float DamagePerBullet;

	/** Current ammount of ammo loaded in the gun ready to be fired*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Ammo")
	int32 LoadedAmmo;

	/** Size of this gun's clip. Each reload will at max put you at this much ammo*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Ammo")
	int32 MaxAmmo;

	/** Ammo not currently loaded in the gun is there*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Ammo")
	int32 ExcessAmmo;

	/** Max ammount of extra ammo that can be held for this weapon*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Ammo")
	int32 MaxExcessAmmo;

	/** Number of seconds this gun has to wait before its ready to shoot again*/
	UPROPERTY(EditAnywhere, Category = "Weapon | Firing")
	float FireRate;

	/** Max distance in Unreal units this gun can shoot*/
	UPROPERTY(EditAnywhere, Category = "Weapon | Firing", meta = (ClampMin = "0.0", ClampMax = "10000.0"))
	float MaxFireRange;

	/** Number of bullets fired for each trigger pull*/
	UPROPERTY(EditAnywhere, Category = "Weapon | Firing", meta = (ClampMin = "1"))
	int32 BulletsPerFire;

	/** Amount of degrees bullets can spread from their orgin point*/
	UPROPERTY(EditAnywhere, Category = "Weapon | Firing", meta = (ClampMin = "0.0", ClampMax = "180.0"))
	float AngularSpread;

	/** How much recoil is applied each time the weapon is fired*/
	UPROPERTY(EditAnywhere, Category = "Weapon | Recoil")
	float Recoil;

	/** Controls how quickly the velocity applied from recoil decays*/
	UPROPERTY(EditAnywhere, Category = "Weapon | Recoil")
	float RecoilFallOff;

	/** Controls amount of recoil active on the weapon. Lower values lower the maximum amount of recoil applied to weapons with rapid fire rates*/
	UPROPERTY(EditAnywhere, Category = "Weapon | Recoil")
	float RecoilLimit;

	/** When aimed, recoil will be this many time effective. This should be on a scale of 0 - 1 */
	UPROPERTY(EditAnywhere, Category = "Weapon | Recoil")
	float RecoilAimFactor;

#if WITH_EDITOR
	UPROPERTY(EditAnywhere, Category = "Weapon | Debug")
	uint8 bDrawFireTraces : 1;
#endif

private:

	/** Aim state of this weapon*/
	uint8 bIsAimed : 1;

	/** True if this weapon can be fired and is not on cooldown from a previous fire*/
	uint8 bCanFire : 1;

	/** Last game time this weapon was fired*/
	float LastFireTimeStamp;

public:

	void Tick(float DeltaTime) override;

	/***	Begin AWeapon Interface   ***/

	virtual bool IsAimable() override { return true; }
	virtual bool IsReloadable() override { return LoadedAmmo < MaxAmmo&& ExcessAmmo > 0; }

	virtual void StartAim() override { bIsAimed = true; }
	virtual void StopAim() override { bIsAimed = false; }

	virtual void Attack() override;
	virtual void Reload() override;

protected:

	virtual void NotifyHUD() override;

	/** Fires the weapon, tracing a rayand applying damage to each actor hit */
	virtual void Fire();

	void AddRecoilVelocity();

	void AddImpulseVelocity();

	/** Blueprint event that provides necessary data for where to draw a cosmetic projectile*/
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Projectile fired"))
	void ReceiveProjectileFired(const FVector& StartLocation, const FVector& EndLocation);
};
