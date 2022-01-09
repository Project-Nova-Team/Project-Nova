#include "AIState.h"
#include "BehaviorTree/BehaviorTree.h"

void UAIState::Initialize(UStateMachine* StateMachine, UObject* ContextObject)
{
	Super::Initialize(StateMachine, ContextObject);
	this->AI = CastContext<AAIBaseController>();
}

void UAIState::OnEnter()
{
	AI->RunBehaviorTree(Tree);
	AI->CurrentState = GetClass()->GetFName().GetPlainNameString();
}
