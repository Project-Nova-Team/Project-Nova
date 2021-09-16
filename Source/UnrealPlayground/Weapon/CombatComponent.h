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
	virtual void SetUpConstruction(USceneComponent* TraceComponent, USkeletalMeshComponent* MeshComponent);

	/** Changes out the current primary weapon with the newly provided weapon*/
	virtual void PickUpNewWeapon(AWeapon* const NewWeapon);

	void InitializeInput(FWeaponInput* WeaponInput) { Input = WeaponInput; }

	/** Returns a pointer to the weapon currently in hand*/
	AWeapon* GetPrimaryWeapon() const { return PrimaryWeapon; }
protected:
	virtual void BeginPlay() override;

	/** The currently active weapon. This is the weapon that will be fired, reloaded, etc.*/
	UPROPERTY(Category = Weapons, EditAnywhere)
	AWeapon* PrimaryWeapon;

	/** The scene component belonging to the owning pawn that we use to track where we initially begin a trace when firing*/
	USceneComponent* TraceOrigin;

	/** The skeletal mesh component that holds the actual weapon mesh on the owning pawn*/
	USkeletalMeshComponent* WeaponMesh;

	/** Input values belonging to the owning pawn used to perform actions on this component (firing, reloading, etc) */
	FWeaponInput* Input;
};
