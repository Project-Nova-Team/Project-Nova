#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIBaseController.generated.h"

class UAIPerceptionComponent;
enum EInstructionState;

UCLASS()
class UNREALPLAYGROUND_API AAIBaseController : public AAIController
{
	GENERATED_BODY()
public:
	AAIBaseController();

protected:
	void BeginPlay() override;

private:
	UPROPERTY(Category = AI, EditAnywhere)
	UBlackboardData* BaseBoard;

	UPROPERTY(Category = AI, EditAnywhere)
	UBehaviorTree* PatrolTree;

	UPROPERTY(Category = AI, EditAnywhere)
	UBehaviorTree* SearchTree;

	UPROPERTY(Category = AI, EditAnywhere)
	UBehaviorTree* AttackTree;

	UPROPERTY(Category = Controller, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UAIPerceptionComponent* Perception;

	UFUNCTION()
	void RunNewTree(EInstructionState NewState);
};
