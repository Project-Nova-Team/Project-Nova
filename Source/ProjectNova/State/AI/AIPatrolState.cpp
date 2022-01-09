#include "AIPatrolState.h"

void UAIPatrolState::Initialize(UStateMachine* StateMachine, UObject* ContextObject)
{
	Super::Initialize(StateMachine, ContextObject);
	Weight = 1;
}

void UAIPatrolState::OnEnter()
{
	Super::OnEnter();
	AI->ClearSearchParameters();
}