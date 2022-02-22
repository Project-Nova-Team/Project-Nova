#pragma once

#include "CoreMinimal.h"
#include "BaseAIController.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "GormController.generated.h"

UCLASS()
class PROJECTNOVA_API AGormController : public ABaseAIController
{
	GENERATED_BODY()
	
protected:

	//These keys are really just byte wrappers so its fine to copy

	/** Key to value bHitStunned*/
	FBlackboard::FKey HitStunnedKey;

	/** Key to value bCanBeStunned*/
	FBlackboard::FKey CanBeStunnedKey;

private:
	static FName HitStunKeyName;
	static FName CanBeStunnedKeyName;

protected:

	/** Determines if the gorm should be hit stunned when damaged by a melee weapon*/
	UFUNCTION()
	void ReceiveTakeDamage(const class UDamageType* DamageType);

	virtual void OwnerRevive() override;

	void OnPossess(APawn* InPawn) override;
};
