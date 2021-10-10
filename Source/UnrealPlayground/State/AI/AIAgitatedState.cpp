#include "AIAgitatedState.h"

void UAIAgitatedState::Initialize(UStateMachine* StateMachine, UObject* ContextObject)
{
	Super::Initialize(StateMachine, ContextObject);
	Weight = 2;
}

void UAIAgitatedState::OnEnter()
{
	//Don't call super here, we dont want to register investigations on agitation!
	AI->CurrentState = GetClass()->GetFName().GetPlainNameString();
	AI->RunBehaviorTree(Tree);
	AI->SetSearchParameters(false);
}