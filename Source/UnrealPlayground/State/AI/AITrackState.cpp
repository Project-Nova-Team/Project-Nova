#include "AITrackState.h"

void UAITrackState::Initialize(UStateMachine* StateMachine, UObject* ContextObject)
{
	Super::Initialize(StateMachine, ContextObject);
	Weight = 4;
}

void UAITrackState::OnEnter()
{
	Super::OnEnter();
	AI->SetSearchParameters(true);
}