#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class AGun;
class AWeapon;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAnimEvent);
DECLARE_MULTICAST_DELEGATE_OneParam(FWeaponCollectionEvent, AWeapon*);

UCLASS()
class UNREALPLAYGROUND_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCombatComponent();

	/** Returns the angular velocity of recoil caused from weapon fire or 0 if no weapon*/
	float GetWeaponRecoilVelocity() const;

	/** Returns the arc (in degrees) that weapon spread can fire about from the origin or NoWeaponBloom if no weapon*/
	float GetWeaponBloom() const;

	/** Returns the rate at which the effect of weapon recoil resets*/
	float GetWeaponRecoilRecovery() const;

	/** Returns the max degrees of recoil offset from a gun*/
	float GetWeaponRecoilLimit() const;

	void SetIsInAnimation(const bool Value) { bIsInAnimation = Value; }

	/** 
	 * An anim instance should call this function with the status of a reload montage
	 * If interrupted, (by something like a swap), we dont want to add ammo to the gun
	 * We want to make sure the animation fully completed to actually give ammo!
	 */
	void ReceiveReloadComplete(const bool bInterrupted);

	/** An anim instance should call this function with the status of a vault/aim/attack montage*/
	void ReceiveAnimationComplete();

	/** An anim instance should call this function with the status of an swap montage*/
	void ReceiveSwapComplete();

	UFUNCTION(BlueprintCallable)
	FORCEINLINE AWeapon* GetHeldWeapon() const { return (Arsenal.Num() > 0) ? Arsenal[CurrentWeaponIndex] : nullptr; }

	bool GetIsAimed() const { return bIsAimed; }

	bool IsActionLocked() const { return (GetHeldWeapon() == nullptr) || bIsReloading || bIsInAnimation || bIsSwapping; }

	bool IsNonSwapLocked() const { return (GetHeldWeapon() == nullptr) || bIsInAnimation || bIsAimed; }

	bool IsAttackLocked() const { return (GetHeldWeapon() == nullptr) || bIsInAnimation || bIsSwapping; }

	bool IsReloadLocked() const { return (GetHeldWeapon() == nullptr) || bIsReloading || bIsInAnimation || bIsAimed; }

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
	void ReceiveAim();

	/** Reloads the weapon*/
	void ReceiveReload();

	FAnimEvent OnAimStart;

	FAnimEvent OnAimStop;

	FAnimEvent OnReload;

	FAnimEvent OnSwap;

	FAnimEvent OnAttack;

	/** Invoked when a new weapon is added to the arensal*/
	FWeaponCollectionEvent OnArsenalAddition;

	/** Invoked when a new weapon is removed to the arensal*/
	FWeaponCollectionEvent OnArsenalRemoval;

protected:
	void BeginPlay() override;

	void SwapEvent();

	/** Collection of weapons on this component*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapons | Arsenal")
	TArray<AWeapon*> Arsenal;

	/** The skeletal mesh component that holds the actual weapon mesh on the owning pawn*/
	USkeletalMeshComponent* WeaponMesh;

	/** The scene component belonging to the owning pawn that we use to track where we initially begin a trace when firing*/
	USceneComponent* TraceOrigin;

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
	UPROPERTY(EditAnywhere, Category = "Weapons | General")
	float NoWeaponRecoilRecovery;

	/** 
	 * Max number of weapons that can exist in the Arsenal. Picking up a new weapon while holding this many weapons
	 * Will drop the currently held weapon for the new one
	 */
	UPROPERTY(EditAnywhere, Category = "Weapons | General")
	uint8 MaxWeaponCount;

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

	/** True if the we are currently playing an animation that would prevent us from peforming other actions*/
	uint8 bIsInAnimation : 1;

	/** Current index of the Arsenal array. The weapon at this index is the currently held weapon*/
	int32 CurrentWeaponIndex;
};
