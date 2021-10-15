#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class AGun;
class AMeleeWeapon;
class AWeapon;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAnimEvent);

UCLASS()
class UNREALPLAYGROUND_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCombatComponent();

	void AddAmmmoToWeapon(AGun* Weapon, int AmmoAddAmount);

	/** Returns the angular velocity of recoil caused from weapon fire or 0 if no weapon*/
	float GetWeaponRecoilVelocity() const;

	/** Returns the arc (in degrees) that weapon spread can fire about from the origin or NoWeaponBloom if no weapon*/
	float GetWeaponBloom() const;

	/** Returns the rate at which the effect of weapon recoil resets*/
	float GetWeaponRecoilRecovery() const;

	bool GetIsAimed() const { return bIsAimed; }

	bool IsActionLocked() const { return bIsReloading || bIsInAnimation || bIsSwapping; }

	void SetUpConstruction(USceneComponent* TraceComponent, USkeletalMeshComponent* MeshComponent);

	void PickUpWeapon(AWeapon* NewWeapon);

	/** Sets the attack state of the weapon*/
	void ReceiveAttack(const bool bAttackEnabled);

	/** Called when owning pawn issues a swap command.
	 *
	 * @param	Direction				Value to index the weapon array by (-1 or 1)
	*/
	void ReceiveSwap(const int32 Direction);

	/** Sets the aim state of the weapon*/
	void ReceiveAim(const bool bAimEnabled);
	
protected:
	void BeginPlay() override;

	/** Collection of weapons on this component*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapons | Arsenal")
	TArray<AWeapon*> Arsenal;

	/** The skeletal mesh component that holds the actual weapon mesh on the owning pawn*/
	USkeletalMeshComponent* WeaponMesh;

	/** The scene component belonging to the owning pawn that we use to track where we initially begin a trace when firing*/
	USceneComponent* TraceOrigin;

	FAnimEvent OnAimStart;

	FAnimEvent OnAimStop;

	FAnimEvent OnReload;

	FAnimEvent OnSwap;

	/** Sets aim state and invokes any necessary events to help with animation*/
	void HandleAimState(const bool bNoWeapon);

	/**
	 * If no weapon is being held, what should the bloom value be?
	 * For the shooter, this will determine the size of the crosshair with no weapon
	 */
	UPROPERTY(EditAnywhere, Category = "Weapons | General")
	float NoWeaponBloom;

	/**
	 * If no weapon is being held, how fast should recoil recover
	 * NOTE: Unless this covers an incredibly unique edge case and is likely never going to be used
	 */
	UPROPERTY(EditAnywhereCategory = "Weapons | General")
	float NoWeaponRecoilRecovery;

	/**
	 * Whether or not the weapon is being aimed down the sights
	 * This determines whether or not bloom applies any effect on the weapon
	 * This is always true when the weapon is being held by AI
	 */
	uint8 bIsAimed : 1;

	/** True if a weapon reload is currently occuring. This will be set false by anim notifies*/
	uint8 bIsReloading : 1;

	/** True if a swap is currently occuring. This will be set false by anim notifies*/
	uint8 bIsSwapping : 1;

	/** True if the we are currently playing a "look down the sights" animation or melee attack animation*/
	uint8 bIsInAnimation : 1;

	/** 
	 * Current index of the Arsenal array. The weapon at this index is the currently held weapon
	 * only using a byte since who really needs more than 256 weapons
	 */
	uint8 CurrentWeaponIndex;
};
