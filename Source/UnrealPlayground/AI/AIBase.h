#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InstructionComponent.h"
#include "AIBase.generated.h"

class USphereComponent;
class UHealthComponent;

UCLASS()
class UNREALPLAYGROUND_API AAIBase : public ACharacter
{
	GENERATED_BODY()

public:
	AAIBase();

	virtual void Tick(float DeltaTime) override;

	UInstructionComponent* GetInstruction() const { return Instruction; }

	/** Called in animation blueprint to enable damage collider*/
	UFUNCTION(BlueprintCallable)
	void OnAttackBegin();

	/** Called in animation blueprint to clear disable damage collider and clear hits*/
	UFUNCTION(BlueprintCallable)
	void OnAttackEnd();

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(Category = Mesh, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* WeaponMesh;

	/** TODO this should be broken out into something more reasonable*/
	UPROPERTY(Category = Combat, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USphereComponent* DamageTrigger;

	UPROPERTY(Category = Instruction, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UInstructionComponent* Instruction;

	UPROPERTY(Category = Health, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UHealthComponent* Health;

private:
	
	/** Called when the damage trigger hits a pawn*/
	UFUNCTION()
	void OnAttackHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/**
	 * Whether or not this attack instance has struck the player. 
	 * Used to ensure we dont hit the player multiple times in one attack
	 */
	uint8 bHasHitPlayer : 1;
};
