#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bullet.generated.h"

class UStaticMeshComponent;
class AWeapon;

UCLASS()
class UNREALPLAYGROUND_API ABullet : public AActor
{
	GENERATED_BODY()

public:
	ABullet();

	void Tick(float DeltaTime) override;

	void InitializeOwner(const float Base, const float Body, const float Limb, const float Head, const float Range, const float FireSpeed);

	void SetBulletQueryParams(const FCollisionQueryParams Params) { QueryParams = Params; }

	/**
	 * Sets the trajectory for the damage trace and bullet then sets the bullet as active
	 *
	 * @param	TraceOrigin				World space location where the fire trace began
	 * @param	TraceVector				The forward vector from a weapon's trace origin to the fire destination
	 * @param	BulletVector			The forward vector from a weapon's barrel to the fire destination
	 */
	void SetTrajectory(const FVector TraceOrigin, const FVector TraceVector, const FVector BulletVector);

	/** Sets the bullet activity which affects tick registration and mesh visibility*/
	void SetIsActive(const bool Value);

	/** Returns true if the projectile is currently in flight*/
	bool GetIsActive() const { return bIsActive; }

protected:
	void BeginPlay() override;
private:

	/** Mesh this bullet actor uses*/
	UPROPERTY(Category = Mesh, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;

	/** Starting location of the trace*/
	FVector TraceStart;

	/** World space position of the current trace*/
	FVector CurrentTraceLocation;

	/** World space direction indicating how the weapon trace should move*/
	FVector TraceDirection;

	/** World space direction indicating how the bullet should move*/
	FVector BulletDirection;

	/** QueryParams used to make sure the owning actor is not collided with*/
	FCollisionQueryParams QueryParams;

	/** True if this projectile is currently in flight*/
	uint8 bIsActive : 1;

	/** How many units each second does the projectile travel*/
	float Speed;

	/** Base amount of damage this bullet deals*/
	float BaseDamage;

	/** Damage multiplier when striking the body*/
	float BodyMultiplier;

	/** Damage multiplier when striking a limb*/
	float LimbMultiplier;

	/** Damage multiplier when striking the head*/
	float HeadMultiplier;

	/** Max distance the trace can travel*/
	float MaxRange;
};