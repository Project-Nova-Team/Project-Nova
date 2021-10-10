#pragma once

#include "CoreMinimal.h"
#include "../Player/WeaponInput.h"
#include "Components/ActorComponent.h"
#include "DrawDebugHelpers.h"
#include "ShooterCombatComponent.generated.h"

class UDelayedActionManager;
class UCameraComponent;
class AGun;
class AMeleeWeapon;
class AWeapon;

struct FDelayedActionHandle;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSwapEvent);

UENUM(BlueprintType)
enum ESwapState
{
	SS_Up  UMETA(DisplayName = "Up"),
	SS_Down UMETA(DisplayName = "Down")
};

UCLASS()
class UNREALPLAYGROUND_API UShooterCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UShooterCombatComponent();

	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void PickUpNewWeapon(AGun* const NewWeapon);

	void PickUpMeleeWeapon(AMeleeWeapon* const NewWeapon);

	// Adds ammo to specific weapon.
	void AddAmmmoToWeapon(AGun* Weapon, int AmmoAddAmount);

	/** Returns the angular velocity of recoil caused from weapon fire or 0 if no weapon*/
	float GetWeaponRecoilVelocity() const;

	/** Returns the arc (in degrees) that weapon spread can fire about from the origin or NoWeaponBloom if no weapon*/
	float GetWeaponBloom() const;

	/** Returns the rate at which the effect of weapon recoil resets*/
	float GetWeaponRecoilRecovery() const;

	bool GetIsAimed() const { return bIsAimed; }

	void SetUpConstruction(USceneComponent* TraceComponent, USkeletalMeshComponent* MeshComponent);

	void ResetLockout();

	void InitializeInput(FWeaponInput* WeaponInput) { Input = WeaponInput; }

	/** Returns a pointer to the weapon currently in hand*/
	AGun* GetPrimaryWeapon() const { return PrimaryGun; }

	/** Enum that holds swap up or swap down, based on scroll direction*/
	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<ESwapState> SwapState;

	/** Switches out the primary and secondary weapons*/
	UFUNCTION(BlueprintCallable)
		void SwapWeapons(TEnumAsByte<ESwapState> SwapDirection);

protected:
	void BeginPlay() override;

private:

	/** The currently active weapon. This is the weapon that will be fired, reloaded, etc.*/
	UPROPERTY(Category = Weapons, EditAnywhere)
	AWeapon* CurrentWeapon;

	/** The currently active weapon. This is the weapon that will be fired, reloaded, etc.*/
	UPROPERTY(Category = Weapons, EditAnywhere)
	AGun* PrimaryGun;

	/** The stored weapon owned by this component that can be switched to from input*/
	UPROPERTY(Category = Weapons, EditAnywhere)
	AGun* OffhandGun;

	UPROPERTY(Category = Weapons, EditAnywhere)
	AMeleeWeapon* MeleeWeapon;

	TArray<AWeapon*> WeaponArray;

	/** The skeletal mesh component that holds the actual weapon mesh on the owning pawn*/
	USkeletalMeshComponent* WeaponMesh;

	/** Input values belonging to the owning pawn used to perform actions on this component (firing, reloading, etc) */
	FWeaponInput* Input;

	/** The scene component belonging to the owning pawn that we use to track where we initially begin a trace when firing*/
	USceneComponent* TraceOrigin;

	/** Delayed Action Manager shorthand pointer*/
	UDelayedActionManager* DelayManager;

	/** Shorthand pointer to the shooter's trace origin casted as a camera*/
	UCameraComponent* Camera;

	void ResetLockoutAfterDelay(const float LockoutDuration);

	/** Helper function that determines whether or not we execute a special actions (throw grenade, use melee, etc)*/
	void HandleSpecialActions();

	/** Helper function that determines whether or not we execute standard actions (aiming, shooting)*/
	void HandleStandardActions(const bool bNoWeapon);

	/** Sets aim state and invokes any necessary events to help with animation*/
	void HandleAimState(const bool bNoWeapon);

	/** Broadcasts event for swap animation*/
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void BroadcastSwapEvent();

	/**Invoked when the shooter swaps weapons*/
	UPROPERTY(BlueprintAssignable)
	FSwapEvent OnWeaponSwapRequest;

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

	// Used to track where we are in the weapons array
	int CurrentWeaponIndex;

	FDelayedActionHandle* Handle;
};
