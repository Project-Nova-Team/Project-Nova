#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AIBase.generated.h"

class UMeleeComponent;
class UHealthComponent;

UCLASS()
class PROJECTNOVA_API AAIBase : public ACharacter
{
	GENERATED_BODY()

public:
	AAIBase();

	UFUNCTION(BlueprintCallable)
	FORCEINLINE UHealthComponent* GetHealth() const { return Health; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE UMeleeComponent* GetMelee() const { return MeleeComponent; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE AActor* GetPlayer() const { return Player; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE AActor* GetPatrolActor() const{ return PatrolActor; }

	/** If true, this AI will start the level inactive. They will be invisible, register no stimulus, and perform no actions until they are enabled*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	uint8 bStartsInActive : 1;

	void SetPlayer(AActor* Value) { Player = Value; }

	/**
	 * This function handles animation, ragdoll and collision
	 * This is actually reported by the controller to reduce overhead on binding a delegate
	 */
	void SetLifeStatus(const bool bIsAlive);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float AttackMoveSpeed;

	float DefaultMoveSpeed;

	void SetVisible(const bool Value);

protected:	

	void BeginPlay() override;

	UPROPERTY(Category = Combat, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UMeleeComponent* MeleeComponent;

	UPROPERTY(Category = Health, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UHealthComponent* Health;

	/** Actor in the level that contains a spline component the AI will patrol*/
	UPROPERTY(EditAnywhere, Category = "Patrol")
	AActor* PatrolActor;

	UPROPERTY(BlueprintReadOnly)
	uint8 bIsDead : 1;

private:

	/** Player object, past down by the owning cell*/
	AActor* Player;
};
