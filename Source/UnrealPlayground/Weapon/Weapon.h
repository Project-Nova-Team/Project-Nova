#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Gameplay/InteractiveObject.h"
#include "Weapon.generated.h"

class USkeletalMesh;
class USkeletalMeshSocket;
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
	virtual bool Attack(const bool bIsAimed) { return false; }

	/** Called by CombatComponent when attack input has been removed*/
	virtual void StopAttack() { }

	/** Sets the aim status of the weapon*/
	virtual void SetAim(const bool bNewAim) { bIsAimed = bNewAim; }

	/**
	* Pointer to a level object that determines where the line trace begins for firing
	* For the shooter, this is the camera component
	*/
	USceneComponent* TraceOrigin;

	void SetWeaponSceneValues(USceneComponent* TraceOriginComponent);

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

	/** Multiplies the base damage by this amount when striking the body*/
	UPROPERTY(EditAnywhere, Category = "Weapon | Damage")
	float BodyMultiplier;

	/** Multiplies the base damage by this amount when striking the head*/
	UPROPERTY(EditAnywhere, Category = "Weapon | Damage")
	float HeadMultiplier;

	/** Multiplies the base damage by this amount when striking a limb*/
	UPROPERTY(EditAnywhere, Category = "Weapon | Damage")
	float LimbMultiplier;

	UCombatComponent* OwningComponent;

	uint8 bIsAimed : 1;
};
