#pragma once

#include "CoreMinimal.h"
#include "CombatComponent.h"
#include "ShooterCombatComponent.generated.h"

class UDelayedActionManager;
class UCameraComponent;

struct FDelayedActionHandle;

UCLASS()
class UNREALPLAYGROUND_API UShooterCombatComponent : public UCombatComponent
{
	GENERATED_BODY()

public:
	UShooterCombatComponent();

	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void PickUpNewWeapon(AWeapon* const NewWeapon) override;

	// Adds ammo to specific weapon.
	void AddAmmmoToWeapon(AWeapon* Weapon, int AmmoAddAmount);

	/** Returns the angular velocity of recoil caused from weapon fire or 0 if no weapon*/
	float GetWeaponRecoilVelocity() const;

	/** Returns the arc (in degrees) that weapon spread can fire about from the origin or NoWeaponBloom if no weapon*/
	float GetWeaponBloom() const;

	/** Returns the rate at which the effect of weapon recoil resets*/
	float GetWeaponRecoilRecovery() const;

	bool GetIsAimed() const { return bIsAimed; }

	void SetUpConstruction(USceneComponent* TraceComponent, USkeletalMeshComponent* MeshComponent) override;

protected:
	void BeginPlay() override;

private:
	/** Switches out the primary and secondary weapons*/
	void SwapWeapons();

	/** The stored weapon owned by this component that can be switched to from input*/
	UPROPERTY(Category = Weapons, EditAnywhere)
	AWeapon* SecondaryWeapon;

	/** Delayed Action Manager shorthand pointer*/
	UDelayedActionManager* DelayManager;

	/** Shorthand pointer to the shooter's trace origin casted as a camera*/
	UCameraComponent* Camera;

	void ResetLockout();

	void ResetLockoutAfterDelay(const float LockoutDuration);

	/** Helper function that determines whether or not we execute a special actions (throw grenade, use melee, etc)*/
	void HandleSpecialActions();

	/** Helper function that determines whether or not we execute standard actions (aiming, shooting)*/
	void HandleStandardActions(const bool bNoWeapon);

	/** Sets aim state and invokes any necessary events to help with animation*/
	void HandleAimState(const bool bNoWeapon);


	/** Brendan Tell me if this is Wrong!!!!*/
	UPROPERTY()
	float StartingCameraFOV;

	/** Once a weapon switch is executed, how long to wait before the weapon is ready to fire?*/
	UPROPERTY(Category = "Weapons | Lockout", EditAnywhere)
	float SwapLockoutTime;

	/** Once a grenade is thrown, how long to wait before we can perform other actions?*/
	UPROPERTY(Category = "Weapons | Lockout", EditAnywhere)
	float ThrowLockoutTime;

	/** Once a melee attack is executed, how long to wait before we can perform other actions?*/
	UPROPERTY(Category = "Weapons | Lockout", EditAnywhere)
	float MeleeLockoutTime;
	
	/**
	 * If no weapon is being held, what should the bloom value be?
	 * For the shooter, this will determine the size of the crosshair with no weapon
	 */
	UPROPERTY(Category = "Weapons | General", EditAnywhere)
	float NoWeaponBloom;

	/**
	 * If no weapon is being held, how fast should recoil recover
	 * NOTE: Unless this covers an incredibly unique edge case and is likely never going to be used
	 */
	UPROPERTY(Category = Weapons, EditAnywhere)
	float NoWeaponRecoilRecovery;

	/**
	 * Whether or not the weapon is being aimed down the sights
	 * This determines whether or not bloom applies any effect on the weapon
	 * This is always true when the weapon is being held by AI
	 */
	uint8 bIsAimed : 1;

	/** Whether or not the component has recently performed an action that would prevent it from performing other actions*/
	uint8 bIsLockedOut;

	FDelayedActionHandle* Handle;
};
