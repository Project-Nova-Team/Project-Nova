#pragma once

#include "CoreMinimal.h"
#include "../State.h"
#include "../../AI/AIBaseController.h"
#include "AIState.generated.h"

class UBehaviorTree;

UCLASS()
class UNREALPLAYGROUND_API UAIState : public UState
{
	GENERATED_BODY()

public:
	UAIState() { }
	~UAIState() { }

	virtual void OnEnter() override;
	virtual void Initialize(UStateMachine* StateMachine, UObject* ContextObject) override;
	void SetTree(UBehaviorTree* Value) { Tree = Value; }

	uint8 Weight;

protected:
	/** The context object as a AI Controller this state is going to perform game logic on*/
	AAIBaseController* AI;

	/** The behavior tree we run while in this state*/
	UBehaviorTree* Tree;
};
