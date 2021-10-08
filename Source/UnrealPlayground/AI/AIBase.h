#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InstructionComponent.h"
#include "AIBase.generated.h"

class USphereComponent;
class UHealthComponent;
class AAICell;

UCLASS()
class UNREALPLAYGROUND_API AAIBase : public ACharacter
{
	GENERATED_BODY()

public:
	AAIBase();
	friend class AICell;

	virtual void Tick(float DeltaTime) override;

	UHealthComponent* GetHealth() const { return Health; }

	AActor* GetPlayer() const { return Player; }

	AActor* GetPatrolActor() const{ return PatrolActor; }

	void SetPlayer(AActor* Value) { Player = Value; }

	/** Called in animation blueprint to enable damage collider*/
	UFUNCTION(BlueprintCallable)
	void OnAttackBegin();

	/** Called in animation blueprint to clear disable damage collider and clear hits*/
	UFUNCTION(BlueprintCallable)
	void OnAttackEnd();

	UFUNCTION()
	void OnTookDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(Category = Mesh, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* WeaponMesh;

	/** TODO this should be broken out into something more reasonable*/
	UPROPERTY(Category = Combat, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USphereComponent* DamageTrigger;

	UPROPERTY(Category = Health, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UHealthComponent* Health;

	/** Actor in the level that contains a spline component the AI will patrol*/
	UPROPERTY(EditAnywhere, Category = "Patrol")
	AActor* PatrolActor;

private:
	
	/** Called when the damage trigger hits a pawn*/
	UFUNCTION()
	void OnAttackHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** Called when the AI takes fatal damage*/
	UFUNCTION()
	void OnDeath();

	/** Player object, past down by the owning cell*/
	AActor* Player;

	/**
	 * Whether or not this attack instance has struck the player. 
	 * Used to ensure we dont hit the player multiple times in one attack
	 */
	uint8 bHasHitPlayer : 1;
};
