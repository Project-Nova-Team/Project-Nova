#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DrawDebugHelpers.h"
#include "../Gameplay/InteractiveObject.h"
#include "Weapon.generated.h"

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

class USkeletalMesh;
class ABullet;
enum EWeaponFireStance;

UCLASS()
class UNREALPLAYGROUND_API AWeapon : public AActor, public IInteractiveObject
{
	GENERATED_BODY()
	
public:	
	AWeapon();
	virtual void Tick(float DeltaTime) override;
	void InteractionEvent(const APawn* EventSender) override;

	/** Fires the weapon in a straight line with no recoil or bloom*/
	void FireStraight();

	/** 
	 * Fires the weapon applying recoil and bloom
	 *
	 * @param	bIsAimed				Whether or not whoever is holding the weapon is aiming the weapon, which determines bloom and recoil
	 */
	void FireWithNoise(const bool bIsAimed, FVector BulletSpawnLoc, FRotator BulletRotation);

	/** Adds to ammo pool. Called by picking up ammo*/
	void AddExcessAmmo(int AmmoAddAmount);

	/** Reloads the weapon*/
	void Reload();

	/** 
	 * Sets the minimum bloom based a weapon stance
	 *
	 * @param	Stance				WeaponFireStance that determines what the base bloom value should be
	 * @param	bIsMoving			Whether or not whoever is holding the weapon is moving, which applies the movementmultiplier to the base
	 */
	void SetBloomMin(const EWeaponFireStance Stance, const bool bIsMoving);

	/** Returns the mesh this weapon uses*/
	USkeletalMesh* GetSkeletalMesh() const { return Mesh->SkeletalMesh; }

	EWeaponFireType GetWeaponType() const { return WeaponFireType; }

	/** Packages relevant information to display to the UI in blueprint*/
	FWeaponUIData GetWeaponUI() const;

	/** Returns the value that controls how quickly the camera returns to its initial position after firing*/
	float GetRecoilRecovery() const { return RecoilRecovery; }

	/** Returns the max amount of angular difference the camera can rotate from its original rotation*/
	float GetRecoilLimit() const { return RecoilAngularLimit; }

	/** Reutnrs the current angular velocity of weapon impulse from firing*/
	float GetRecoilVelocity() const { return RecoilVelocity; }

	float GetBloom() const { return CurrentBloom; }

	/** 
	 * Sets the owning pawn and sets up values the weapon needs
	 * 
	 * @param	TraceOriginComponent	A scene component used to determine where the firing trace begins
	 */
	void SetTraceOrigin(const USceneComponent* TraceOriginComponent);

	/** Field of view when zoomed in using this weapon*/
	UPROPERTY(EditAnywhere, Category = "Weapon | Aiming")
	float AimFOV;

protected:
	
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	USkeletalMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, Category = "Weapon | General")
	TEnumAsByte<EWeaponFireType> WeaponFireType;

	/** The max range this weapon can be fired*/
	UPROPERTY(VisibleAnywhere, Category = "Weapon | General")
	float MaxFireRange;

	/** Force applied to weapon when dropped by a pawn*/
	UPROPERTY(EditAnywhere, Category = "Weapon | General")
	float ThrowForce;

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

	/** The amount of damage dealt by each attack withs this weapon*/
	UPROPERTY(EditAnywhere, Category = "Weapon | Damage")
	float BaseDamage;

	/** Multiplies the base damage by this amount when striking the body*/
	UPROPERTY(EditAnywhere, Category = "Weapon | Damage")
	float BodyMultiplier;

	/** Multiplies the base damage by this amount when striking the head*/
	UPROPERTY(EditAnywhere, Category = "Weapon | Damage")
	float HeadMultiplier;

	/** Multiplies the base damage by this amount when striking a limb*/
	UPROPERTY(EditAnywhere, Category = "Weapon | Damage")
	float LimbMultiplier;

	/** How many seconds it takes for this weapon to be ready to fire again*/
	UPROPERTY(EditAnywhere, Category = "Weapon | Firing")
	float FireRate;

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
	 * How much the this weapon's recoil multiplied by when it is being aimed?
	 * AI have no recoil so this value does not matter for them
	 */
	UPROPERTY(EditAnywhere, Category = "Weapon | Firing")
	float RecoilAimFactor;

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

//Begin Multicast Delegate Events

	/** Invoked when the weapon fires*/
	UPROPERTY(BlueprintAssignable)
	FWeaponEvent OnWeaponFire;

	/** Invoked when the weapon is prompted to reload*/
	UPROPERTY(BlueprintAssignable)
	FWeaponEvent OnWeaponReload;

private:

	/** Alters the angular velocity of camera rotation caused by weapon spread*/
	void AddRecoilVelocity(const float Velocity);

	/** Alters the amount of weapon spread applied to the weapon*/
	void AddBloom(const float BloomAmount);

	/** 
	 * Pointer to a level object that determines where the line trace begins for firing
	 * For the shooter, this is the camera component
	 */
	const USceneComponent* TraceOrigin;

	/** Parameters used during line tracing*/
	FCollisionQueryParams QueryParams;

	/** Whether or not the weapon is ready to fire*/
	uint8 bCanFire : 1;

	/** Used to track if the weapon is ready to fire*/
	float FireTimer;

	/** Value used to determine how much to sway the camera by*/
	float RecoilVelocity;

	/** Current amount of bloom applied to the primary weapon (in degrees)*/
	float CurrentBloom;

	/** Current minimum amount of bloom*/
	float BloomMin;
};
