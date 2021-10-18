#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.h"
#include "Gun.generated.h"

UENUM()
enum EWeaponFireType
{
	FT_Auto,
	FT_Semi,
	FT_Burst
};

USTRUCT(BlueprintType)
struct FGunUIData : public FWeaponUIData
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

	void StartAttack() override { bAttacking = true; }

	void StopAttack() override { bAttacking = false; }

	bool IsReloadable() override;

	bool IsAimable() override { return true; }

	/** Packages relevant information to display to the UI in blueprint*/
	FGunUIData GetGunUI() const;

	/** Adds to ammo pool. Called when picking up ammo*/
	void AddExcessAmmo(int AmmoAddAmount);

	/** Reloads the weapon*/
	void Reload() override;

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

	void SetWeaponSceneValues(USceneComponent* TraceOriginComponent, USkeletalMeshComponent* ProjectileOriginMesh) override;

	EWeaponFireType GetWeaponType() const { return WeaponFireType; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Fires a bullet in a straight line with no bloom*/
	virtual void FireStraight();

	/** Fires the weapon applying recoil and bloom*/
	virtual void FireWithNoise();

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

	/** True if the owning combat component has successfully issued a fire command*/
	uint8 bAttacking : 1;


	/** Template class of bullet actor we use for this weapon*/
	UPROPERTY(EditAnywhere, Category = "Weapon | General")
	TSubclassOf<ABullet> BulletTemplate;

	UPROPERTY(EditAnywhere, Category = "Weapon | General")
	TEnumAsByte<EWeaponFireType> WeaponFireType;

	/** Field of view when zoomed in using this weapon*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | General")
	float AimFOV;

	/** How many bullets are pooled on begin play*/
	UPROPERTY(VisibleAnywhere, Category = "Weapon | General")
	int16 StartingPoolSize;

	/** Need every weapon to have a socket with this name*/
	UPROPERTY(VisibleAnywhere, Category = "Weapon | General")
	FName BarrelSocketName;

	/** How many seconds it takes for this weapon to be ready to fire again*/
	UPROPERTY(EditAnywhere, Category = "Weapon | Firing")
	float FireRate;

	/** The max range this weapon can be fired*/
	UPROPERTY(EditAnywhere, Category = "Weapon | Firing", meta = (ClampMax = "10000.0"))
	float MaxFireRange;

	/**
	 * How much recoil is applied each time the weapon is fired
	 * When held by AI this will always be zero
	 */
	UPROPERTY(EditAnywhere, Category = "Weapon | Firing")
	float Recoil;

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

	/** How much bloom is added to the base when the weapon is fired from the hip*/
	UPROPERTY(EditAnywhere, Category = "Weapon | Firing")
	float Bloom;

	/** The max value of bloom possible*/
	UPROPERTY(VisibleAnywhere, Category = "Weapon | Firing")
	float BloomMax;

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

	/** How many units each second does the projectile cover*/
	UPROPERTY(EditAnywhere, Category = "Weapon | Firing")
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

	/** Multiplies the base damage by this amount when striking the body*/
	UPROPERTY(EditAnywhere, Category = "Weapon | Damage")
	float BodyMultiplier;

	/** Multiplies the base damage by this amount when striking the head*/
	UPROPERTY(EditAnywhere, Category = "Weapon | Damage")
	float HeadMultiplier;

	/** Multiplies the base damage by this amount when striking a limb*/
	UPROPERTY(EditAnywhere, Category = "Weapon | Damage")
	float LimbMultiplier;

	/** If true, enables line traces showing fire direction*/
	UPROPERTY(EditAnywhere, Category = "Weapon | Debug")
	uint8 bTraceDebug : 1;

	ABullet* GetAvailableBullet();

	/** Alters the angular velocity of camera rotation caused by weapon spread*/
	void AddRecoilVelocity(const float Velocity);

	/** Alters the amount of weapon spread applied to the weapon*/
	void AddBloom(const float BloomAmount);

	/** Short hand pointer to the socket attached to the HeldWeaponMesh at the barrel*/
	const USkeletalMeshSocket* BulletOrigin;

	/** Parameters used during line tracing*/
	FCollisionQueryParams QueryParams;

private:

	//Maybe this should be static so every gun can pull from a single pool?

	/** Object pool of bullet actors we access when firing this weapon*/
	TArray<ABullet*> BulletPool;
};
