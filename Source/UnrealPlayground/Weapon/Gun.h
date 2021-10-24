// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.h"
#include "Gun.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWeaponEvent);

UENUM()
enum EWeaponFireType
{
	FT_Auto,
	FT_Semi,
	FT_Burst
};

USTRUCT(BlueprintType)
struct FWeaponUIData
{
	GENERATED_BODY()

public:
	UPROPERTY()
		uint16 AmmoInClip;

	UPROPERTY()
		uint16 ExcessAmmo;

	UPROPERTY()
		uint16 ClipSize;
};


class AWeapon;
class USkeletalMesh;
class USkeletalMeshSocket;
class ABullet;
enum EWeaponFireStance;

UCLASS()
class UNREALPLAYGROUND_API AGun : public AWeapon
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGun();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void InteractionEvent(const APawn* EventSender) override;

	/** Fires the weapon in a straight line with no recoil or bloom*/
	void FireStraight();

	/**
	 * Fires the weapon applying recoil and bloom
	 *
	 * @param	bIsAimed				Whether or not whoever is holding the weapon is aiming the weapon, which determines bloom and recoil
	 * @param	BulletRotation			Orientation the spawned actor begins in
	 */
	void FireWithNoise(const bool bIsAimed, FRotator BulletRotation);

	/**
	 * Fires the weapon applying recoil and bloom
	 *
	 * @param	bIsAimed				Whether or not whoever is holding the weapon is aiming the weapon, which determines bloom and recoil
	 * @param	BulletRotation			Orientation the spawned actor begins in
	 */
	void FireShotgun(const bool bIsAimed, FRotator BulletRotation);

	/** Packages relevant information to display to the UI in blueprint*/
	FWeaponUIData GetWeaponUI() const;

	/** Adds to ammo pool. Called by picking up ammo*/
	void AddExcessAmmo(int AmmoAddAmount);

	/** Reloads the weapon*/
	UFUNCTION(BlueprintCallable)
	void Reload();

	int GetExcessAmmo() const { return ExccessAmmo; }

	int GetMaxHeldAmmo() const { return MaxHeldAmmo; }

	/** Reutnrs the current angular velocity of weapon impulse from firing*/
	float GetRecoilVelocity() const { return RecoilVelocity; }

	float GetBloom() const { return CurrentBloom; }

	/** Returns the value that controls how quickly the camera returns to its initial position after firing*/
	float GetRecoilRecovery() const { return RecoilRecovery; }

	/** Returns the max amount of angular difference the camera can rotate from its original rotation*/
	float GetRecoilLimit() const { return RecoilAngularLimit; }

	/**
	 * Sets the minimum bloom based a weapon stance
	 *
	 * @param	Stance				WeaponFireStance that determines what the base bloom value should be
	 * @param	bIsMoving			Whether or not whoever is holding the weapon is moving, which applies the movementmultiplier to the base
	 */
	void SetBloomMin(const EWeaponFireStance Stance, const bool bIsMoving);

	/**
	 * Sets the owning pawn and sets up values the weapon needs
	 *
	 * @param	TraceOriginComponent	A scene component used to determine where the firing trace begins
	 * @param	HeldWeapon				The skeletal mesh component of the owning actor
	 * @param	BulletSocket			Socket at the barrel of the gun where bullets spawn
	 */
	void SetGunSceneValues(const USceneComponent* TraceOriginComponent, const USkeletalMeshComponent* HeldWeapon, const USkeletalMeshSocket* BulletSocket);

	EWeaponFireType GetWeaponType() const { return WeaponFireType; }

	int GetAmmoCount() { return CurrentAmmo; }

	int GetClipSize() { return ClipSize; }

	/** Field of view when zoomed in using this weapon*/
	UPROPERTY(EditAnywhere, Category = "Weapon | Aiming")
		float AimFOV;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Used to track if the weapon is ready to fire*/
	float FireTimer;

	/** Value used to determine how much to sway the camera by*/
	float RecoilVelocity;

	/** Current amount of bloom applied to the primary weapon (in degrees)*/
	float CurrentBloom;

	/** Current minimum amount of bloom*/
	float BloomMin;

	/** Whether or not the weapon is ready to fire*/
	uint8 bCanFire : 1;

	/** How many seconds it takes for this weapon to be ready to fire again*/
	UPROPERTY(EditAnywhere, Category = "Weapon | Firing")
		float FireRate;

	/**
	* This value controls how quickly the camera position resets after firing
	* AI have no recoil so this value does not matter for them
	*/
	UPROPERTY(EditAnywhere, Category = "Weapon | Firing")
		float RecoilRecovery;

	/**
	 * This value controls many degrees recoil can displace us from the original look direction
	 * AI have no recoil so this value does not matter for them
	 */
	UPROPERTY(EditAnywhere, Category = "Weapon | Firing")
		float RecoilAngularLimit;
	
	/**
	 * How much recoil is applied each time the weapon is fired
	 * When held by AI this will always be zero
	 */
	UPROPERTY(EditAnywhere, Category = "Weapon | Firing")
		float Recoil;

	/**
	 * This value controls how quickly impulse from weapon fire decays
	 * AI have no recoil so this value does not matter for them
	 */
	UPROPERTY(EditAnywhere, Category = "Weapon | Firing")
		float RecoilFallOff;

	/**
	 * How much the this weapon's recoil multiplied by when it is being aimed?
	 * AI have no recoil so this value does not matter for them
	 */
	UPROPERTY(EditAnywhere, Category = "Weapon | Firing")
		float RecoilAimFactor;

	UPROPERTY(EditAnywhere, Category = "Weapon | General")
		TEnumAsByte<EWeaponFireType> WeaponFireType;

	/** The max value of bloom possible*/
	UPROPERTY(VisibleAnywhere, Category = "Weapon | Firing")
		float BloomMax;

	/** How much bloom is added to the base when the weapon is fired from the hip*/
	UPROPERTY(EditAnywhere, Category = "Weapon | Firing")
		float Bloom;

	/** How quickly bloom decays when over the base value*/
	UPROPERTY(EditAnywhere, Category = "Weapon | Firing")
		float BloomFallOff;

	/** The base amount of bloom this weapon has in the walking state*/
	UPROPERTY(EditAnywhere, Category = "Weapon | Firing")
		float BloomWalkBase;

	/**  The base amount of bloom this weapon has in the crouching state*/
	UPROPERTY(EditAnywhere, Category = "Weapon | Firing")
		float BloomCrouchBase;

	/** The base amount of bloom this weapon has in the proning state*/
	UPROPERTY(EditAnywhere, Category = "Weapon | Firing")
		float BloomProneBase;

	/** Multiplies the bloom base by this much when moving*/
	UPROPERTY(EditAnywhere, Category = "Weapon | Firing")
		float BloomBaseMovementMultiplier;

	/** If true, enables line traces showing fire direction*/
	UPROPERTY(EditAnywhere, Category = "Weapon | Debug")
		uint8 bTraceDebug : 1;

	/** The max range this weapon can be fired*/
	UPROPERTY(VisibleAnywhere, Category = "Weapon | General")
		float MaxFireRange;

	/** How many units each second does the projectile cover*/
	UPROPERTY(EditAnywhere, Category = "Weapon | General")
		float ProjectileSpeed;

	/** The current amount of ammo in the clip*/
	UPROPERTY(EditAnywhere, Category = "Weapon | Ammo")
		uint16 CurrentAmmo;

	/** The max amount of ammo this weapon can hold in a single clip*/
	UPROPERTY(EditAnywhere, Category = "Weapon | Ammo")
		uint16 ClipSize;

	/** Current amount of excess ammo this weapon has attached*/
	UPROPERTY(EditAnywhere, Category = "Weapon | Ammo")
		uint16 ExccessAmmo;

	/** The max amount of the shooter can hold of this weapon*/
	UPROPERTY(EditAnywhere, Category = "Weapon | Ammo")
		uint16 MaxHeldAmmo;

	/** Template class of bullet actor we use for this weapon*/
	UPROPERTY(EditAnywhere, Category = "Mesh")
		TSubclassOf<ABullet> BulletTemplate;

	//Begin Multicast Delegate Events

	/** Invoked when the weapon fires*/
	UPROPERTY(BlueprintAssignable)
		FWeaponEvent OnWeaponFire;

	/** Invoked when the weapon is prompted to reload*/
	UPROPERTY(BlueprintAssignable)
		FWeaponEvent OnWeaponReload;

private:
	ABullet* GetAvailableBullet();

	/** Alters the angular velocity of camera rotation caused by weapon spread*/
	void AddRecoilVelocity(const float Velocity);

	/** Alters the amount of weapon spread applied to the weapon*/
	void AddBloom(const float BloomAmount);

	/** Short hand pointer to the socket attached to the HeldWeaponMesh at the barrel*/
	const USkeletalMeshSocket* BulletOrigin;

	/** Parameters used during line tracing*/
	FCollisionQueryParams QueryParams;

	/** Object pool of bullet actors we access when firing this weapon*/
	TArray<ABullet*> BulletPool;

	/** How many bullets are pooled on begin play*/
	UPROPERTY(VisibleAnywhere, Category = "Weapon | General")
		int16 StartingPoolSize;

};
