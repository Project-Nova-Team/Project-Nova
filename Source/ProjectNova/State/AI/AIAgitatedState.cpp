#include "AIAgitatedState.h"

void UAIAgitatedState::Initialize(UStateMachine* StateMachine, UObject* ContextObject)
{
	Super::Initialize(StateMachine, ContextObject);
	Weight = 2;
}