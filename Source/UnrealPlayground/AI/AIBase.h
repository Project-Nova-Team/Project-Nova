#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InstructionComponent.h"
#include "AIBase.generated.h"

class UMeleeComponent;
class UHealthComponent;

UCLASS()
class UNREALPLAYGROUND_API AAIBase : public ACharacter
{
	GENERATED_BODY()

public:
	AAIBase();

	FORCEINLINE UHealthComponent* GetHealth() const { return Health; }

	FORCEINLINE UMeleeComponent* GetMelee() const { return MeleeComponent; }

	FORCEINLINE AActor* GetPlayer() const { return Player; }

	FORCEINLINE AActor* GetPatrolActor() const{ return PatrolActor; }

	void SetPlayer(AActor* Value) { Player = Value; }

	/**
	 * This function handles animation, ragdoll and collision
	 * This is actually reported by the controller to reduce overhead on binding a delegate
	 */
	void SetLifeStatus(const bool bIsAlive);

protected:	
	UPROPERTY(Category = Combat, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UMeleeComponent* MeleeComponent;

	UPROPERTY(Category = Health, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UHealthComponent* Health;

	/** Actor in the level that contains a spline component the AI will patrol*/
	UPROPERTY(EditAnywhere, Category = "Patrol")
	AActor* PatrolActor;

private:

	/** Player object, past down by the owning cell*/
	AActor* Player;
};
