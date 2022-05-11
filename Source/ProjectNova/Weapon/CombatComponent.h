#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Weapon.h"
#include "CombatComponent.generated.h"

DECLARE_DELEGATE(FAnimationRequest);
DECLARE_DELEGATE_OneParam(FWeaponAddEvent, AWeapon*);
DECLARE_MULTICAST_DELEGATE_OneParam(FLocomotionEvent, const AWeapon*);
DECLARE_DELEGATE_OneParam(FNotifyWeaponHUD, const FWeaponHUD&);

UCLASS()
class PROJECTNOVA_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UCombatComponent();

	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Global socket name combat components use to attach weapons to*/
	static FName WeaponSocketName;

	/** Secondary socket. Used for two handed weapons*/
	static FName SecondarySocketName;

	/** Delegate that notifies owning pawn to update it's HUD because a weapon requested it to*/
	FNotifyWeaponHUD OnUpdateHUD;

	/** Executed on receiving Aim Start command*/
	FAnimationRequest OnAimStart;

	/** Executed on receiving Aim Stop command*/
	FAnimationRequest OnAimStop;

	/** Executed on receiving Reload command*/
	FAnimationRequest OnReload;

	/** Executed on receiving Swap command*/
	FAnimationRequest OnSwap;

	/** Executed on receiving a command that cancels animation*/
	FAnimationRequest OnAnimCancel;

	/** Executed when a weapon switch occurs*/
	FLocomotionEvent OnLocomotionChange;

	/** Executed when a weapon gets added to the arsenal*/
	FWeaponAddEvent OnWeaponAdd;

	void MarkInAnimation() { bIsInAnimation = true; }

	/** Returns a reference to the first weapon of the provided class in Arsenal or nullptr if not found*/
	AWeapon* GetWeaponOfType(TSubclassOf<AWeapon> WeaponClass);

protected:

	/** Collection of weapons on this component*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat | Arsenal")
	TArray<AWeapon*> Arsenal;

	/**
	 * Max number of weapons that can exist in the Arsenal. Picking up a new weapon while holding this many weapons
	 * Will drop the currently held weapon for the new one
	 */
	UPROPERTY(EditAnywhere, Category = "Combat | General")
	int32 MaxWeaponCount;

private:

	/** Weapons that are picked up will be attached to this mesh (usually the owning pawn's character mesh)*/
	USkeletalMeshComponent* AttachmentMesh;

	/** Scene component used to perform HitScan tests (usually the camera)*/
	USceneComponent* TraceOrigin;

	/** Current index of the Arsenal array. The weapon at this index in arsenal is the currently held weapon*/
	int32 CurrentWeaponIndex;


	// Input State //

	/** True if a weapon reload is currently occuring.*/
	uint8 bIsReloading : 1;

	/** True if a swap is currently occuring.*/
	uint8 bIsSwapping : 1;

	/** True if the held weapon is aimable and in an aim state*/
	uint8 bIsAimed : 1;

	/** True if the we are currently playing an animation that would prevent us from peforming other actions*/
	uint8 bIsInAnimation : 1;

	/** True if the attack binding is currently being held down*/
	uint8 bIsTryingToAttack : 1;

	/** True if the owner is running which prevents attacks*/
	const bool* bIsRunning;

	/** Whether or not this component will update the HUD*/
	uint8 bUpdatesHUD : 1;

public:

	/** Helper function that should be called by the owning actor after adding this component in the constructor
	 *	This enables picked up weapons to attach themselves to their owners socket and get information for projectile
	 *  traces. Ex. A gun firing a bullet
	 *
	 * @param	SocketMesh		The mesh you want any picked up weapons to attach themselves to
	 * @param	TraceComponent	A scene component (usually a camera) you want to perform hit scan tests with. Useful for gun firing
	 * @param	bRunningFlag	Pointer to field denoting the active status of the owner's run state
	 */
	void SetUpConstruction(USkeletalMeshComponent* SocketMesh, USceneComponent* TraceComponent, const bool* bRunningFlag);

	/** Adds a new weapon to this combat component's Arsenal*/
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void PickUpWeapon(AWeapon* NewWeapon);

	/** Switches currently held weapon to a new one
	 *	Adjusts CurrentWeaponIndex by the specified direction.
	 *  This should usually be called with parameters of (-1, 1)
	 */
	void SwapWeapon(const int32 Direction);

	/** Returns the weapon currently being held. Returns null if there are none*/
	UFUNCTION(BlueprintCallable, Category = "Combat")
	AWeapon* GetHeldWeapon() const;

	/** Returns component responsible for mesh traces*/
	FORCEINLINE USceneComponent* GetTraceOrigin() const { return TraceOrigin; }

	FORCEINLINE USkeletalMeshComponent* GetAttachmentMesh() const { return AttachmentMesh; }

	/** Returns whether or not this component updates it's owner's HUD*/
	FORCEINLINE bool GetUpdatesHUD() const { return bUpdatesHUD; }

protected:

	void BeginPlay() override;

private:

	FORCEINLINE bool IsActionLocked() const { return (GetHeldWeapon() == nullptr) || *bIsRunning || bIsReloading || bIsInAnimation || bIsSwapping; }

	FORCEINLINE bool IsNonSwapLocked() const { return (GetHeldWeapon() == nullptr) || bIsInAnimation || bIsAimed; }

	FORCEINLINE bool IsReloadLocked() const { return (GetHeldWeapon() == nullptr) || bIsReloading || bIsInAnimation || bIsAimed; }

	void NotifyWeaponChange();

public:

	void ReceiveReload();

	void ReceiveStartAttack();

	void ReceiveStopAttack();

	void ReceiveAimStart();

	void ReceiveAimStop();

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

	/** Executed by a weapon in this components Arsenal when it is requesting a HUD update*/
	void UpdateHUD();
};