#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DrawDebugHelpers.h"
#include "../Gameplay/InteractiveObject.h"
#include "Weapon.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWeaponEvent);

UENUM()
enum EWeaponClass 
{
	WC_SubMachineGun,
	WC_AutomaticRifle,
	WC_SniperRifle,
	WC_LightMachineGun,
	WC_Shotgun,
	WC_Pistol
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
class UCombatComponent;
enum EWeaponFireStance;

UCLASS()
class UNREALPLAYGROUND_API AWeapon : public AActor, public IInteractiveObject
{
	GENERATED_BODY()
	
public:	
	AWeapon();
	virtual void Tick(float DeltaTime) override;
	void InteractionEvent(const APawn* EventSender) override;

	friend class UCombatComponent;

	/** Fires the weapon*/
	void Fire();

	void Reload();

	/** Returns the combat component this weapon belongs to*/
	UCombatComponent* GetOwningComponent() const { return OwningComponent; }

	/** Returns the mesh this weapon uses*/
	USkeletalMesh* GetSkeletalMesh() const { return Mesh->SkeletalMesh; }

	/** Packages relevant information to display to the UI in blueprint*/
	FWeaponUIData GetWeaponUI() const;

	/** Returns the value that controls how quickly the camera returns to its initial position after firing*/
	float GetRecoilRecovery() const { return RecoilRecovery; }

	/** Returns the max amount of angular difference the camera can rotate from its original rotation*/
	float GetRecoilLimit() const { return RecoilAngularLimit; }

	/** 
	 * Sets the owning pawn and sets up values the weapon needs
	 * 
	 * @param	NewOwner				The new pawn the weapon belongs to
	 * @param	TraceOriginComponent	A scene component used to determine where the firing trace begins
	 **/
	void SetOwningComponent(UCombatComponent* NewOwner,  const USceneComponent* TraceOriginComponent);

protected:
	
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	USkeletalMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, Category = "Weapon | General")
	TEnumAsByte<EWeaponClass> WeaponType;

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
	 * This value controls how quickly the camera positions resets after firing
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

	/** The combat component this weapon belongs to*/
	UCombatComponent* OwningComponent;

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
};
