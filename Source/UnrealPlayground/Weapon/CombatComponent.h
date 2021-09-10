#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Weapon.h"
#include "../Player/WeaponInput.h"
#include "CombatComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREALPLAYGROUND_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/**
	 * Assigns appropriate component values so design doesn't forget to
	 * The owning pawn should call this in the constructor immediatly after creating the component
	 * 
	 * @param	TraceComponent				The scene component (probably the camera) we use to fire weapon shots from
	 * @param	MeshComponent				The weapon mesh of the owning pawn
	 */
	void SetUpConstruction(USceneComponent* TraceComponent, USkeletalMeshComponent* MeshComponent);

	/** Changes out the current primary weapon with the newly provided weapon*/
	void PickUpNewWeapon(AWeapon* const NewWeapon);

	void InitializeInput(FWeaponInput* WeaponInput) { Input = WeaponInput; }

	/** Returns a pointer to the weapon currently in hand*/
	AWeapon* GetPrimaryWeapon() const { return PrimaryWeapon; }

	float GetRecoilVelocity() const { return RecoilVelocity; }

	float GetCurrentBloom() const { return CurrentBloom; }

	bool IsAimed() const { return bIsAimed; }

	/** Applies additional recoil to this component*/
	void AddRecoilVelocity(const float Velocity);

	/** Applies additional bloom to this component*/
	void AddBloom(const float Bloom);

protected:

	virtual void BeginPlay() override;

	/** Switches out the primary and secondary weapons*/
	void SwapWeapons();

private:

	/** The currently active weapon. This is the weapon that will be fired, reloaded, etc.*/
	UPROPERTY(Category = Weapons, EditAnywhere)
	AWeapon* PrimaryWeapon;

	/** The stored weapon owned by this component that can be switched to from input*/
	UPROPERTY(Category = Weapons, EditAnywhere)
	AWeapon* SecondaryWeapon;

	/** Once a weapon switch is executed, how long to wait before the weapon is ready to fire?*/
	UPROPERTY(Category = Weapons, EditAnywhere)
	float SwapLockoutTime;

	/** If no weapon is being held, what should recoil fall off default to*/
	UPROPERTY(Category = Weapons, EditAnywhere)
	float NoWeaponRecoilFallOff;

	/** If no weapon is being held, what should bloom fall off default to*/
	UPROPERTY(Category = Weapons, EditAnywhere)
	float NoWeaponBloomFallOff;

	/** 
	 * If no weapon is being held, what should the bloom value be?
	 * For the shooter, this will determine the size of the crosshair with no weapon
	 */
	UPROPERTY(Category = Weapons, EditAnywhere)
	float NoWeaponBloom;

	/**The scene component belonging to the owning pawn that we use to track where we initially begin a trace when firing*/
	USceneComponent* TraceOrigin;

	/**The skeletal mesh component that holds the actual weapon mesh on the owning pawn*/
	USkeletalMeshComponent* WeaponMesh;

	/**Input values belonging to the owning pawn used to perform actions on this component (firing, reloading, etc) */
	FWeaponInput* Input;

	/** Whether or not the component has just swapped primary and secondary weapons and needs some time before ready to fire*/
	uint8 bIsInSwapLockout;

	/**
	 * Whether or not the weapon is being aimed down the sights
	 * This determines whether or not bloom applies any effect on the weapon
	 * This is always true when the weapon is being held by AI
	 */
	uint8 bIsAimed : 1;

	/** Value used to determine how much to sway the camera by*/
	float RecoilVelocity;

	/** Current amount of bloom applied to the primary weapon (in degrees)*/
	float CurrentBloom;
	
	/** Returns true if the weapon isn't blocked other actions like reloading and swapping*/
	bool CanFire();
};
