#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Gameplay/InteractiveObject.h"
#include "Weapon.generated.h"

class USkeletalMesh;
class USkeletalMeshSocket;
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

	/** Returns the mesh this weapon uses*/
	FORCEINLINE USkeletalMeshComponent* GetMeshComponent() const { return Mesh; }

	/** Returns the mesh's skeletal mesh this weapon uses*/
	FORCEINLINE USkeletalMesh* GetSkeletalMesh() const { return Mesh->SkeletalMesh; }

	/** Returns the combat component this weapon is attached to, if any*/
	FORCEINLINE UCombatComponent* GetOwningComponent() const { return OwningComponent; }

	/** Initiates an attack. Returns true if the attack was executed successfully.*/
	virtual void StartAttack() { }

	/** Called by CombatComponent when attack input has been removed*/
	virtual void StopAttack() { }

	/** Sets the aim status of the weapon*/
	virtual void SetAim(const bool bNewAim) { bIsAimed = bNewAim; }

	/**
	* Pointer to a level object that determines where the line trace begins for firing
	* For the shooter, this is the camera component
	*/
	USceneComponent* TraceOrigin;

	/**
	 * Pointer to a the skeletal mesh component of the weapon's owner. This is provided
	 * so we can access a socket on the mesh to fire a cosmetic projectile from 
	 */
	USkeletalMeshComponent* ProjectileOrigin;

	/**
	 * Passes references of scene components to the weapon so it knows where to begin weapon fire/traces from
	 * 
	 * @param	TraceOriginComponent				Scene component a traveling hitscan beam is fired from. Typically the camera of a player
	 * @param	ProjectileOriginMesh				Scene component we use to get a socket on a gun barrel to fire a cosmetic projectile from
	 */
	void SetWeaponSceneValues(USceneComponent* TraceOriginComponent, USkeletalMeshComponent* ProjectileOriginMesh);

protected:
	
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	USkeletalMeshComponent* Mesh;

	/** Force applied to weapon when dropped by a pawn*/
	UPROPERTY(EditAnywhere, Category = "Weapon | General")
	float ThrowForce;

	/** The amount of damage dealt by each attack withs this weapon*/
	UPROPERTY(EditAnywhere, Category = "Weapon | Damage")
	float BaseDamage;

	UCombatComponent* OwningComponent;

	uint8 bIsAimed : 1;
};
