#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DrawDebugHelpers.h"
#include "../Gameplay/InteractiveObject.h"
#include "ShooterCombatComponent.h"
#include "Engine/World.h"
#include "Weapon.generated.h"

class USkeletalMesh;
class USkeletalMeshSocket;
class ABullet;
enum EWeaponFireStance;

UCLASS()
class UNREALPLAYGROUND_API AWeapon : public AActor, public IInteractiveObject
{
	GENERATED_BODY()
	
public:	
	AWeapon();
	virtual void Tick(float DeltaTime) override;
	void InteractionEvent(const APawn* EventSender) override;

	void SetInteractiveObjectHidden(bool ActiveState) override;

	/** Returns the mesh this weapon uses*/
	USkeletalMeshComponent* GetMeshComponent() const { return Mesh; }

	/** Returns the mesh's skeletal mesh this weapon uses*/
	USkeletalMesh* GetSkeletalMesh() const { return Mesh->SkeletalMesh; }

	void SetHeldWeaponMesh(const USkeletalMeshComponent* MeshToSet);

	void SetActorTick(bool status);

	/**
	* Pointer to a level object that determines where the line trace begins for firing
	* For the shooter, this is the camera component
	*/
	const USceneComponent* TraceOrigin;

	void SetWeaponSceneValues(const USceneComponent* TraceOriginComponent, const USkeletalMeshComponent* HeldWeapon);

protected:
	
	virtual void BeginPlay() override;

	/** Force applied to weapon when dropped by a pawn*/
	UPROPERTY(EditAnywhere, Category = "Weapon | General")
		float ThrowForce;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	USkeletalMeshComponent* Mesh;

	/** Short hand pointer to the mesh component we are holding*/
	const USkeletalMeshComponent* HeldWeaponMesh;

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
};
