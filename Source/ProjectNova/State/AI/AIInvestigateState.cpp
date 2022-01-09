#include "AIInvestigateState.h"

void UAIInvestigateState::Initialize(UStateMachine* StateMachine, UObject* ContextObject)
{
	Super::Initialize(StateMachine, ContextObject);
	Weight = 3;
}

void UAIInvestigateState::OnEnter()
{
	Super::OnEnter();
	AI->SetSearchParameters(false);
}