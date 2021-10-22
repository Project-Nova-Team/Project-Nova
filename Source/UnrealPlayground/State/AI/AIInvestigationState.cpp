#include "AIInvestigationState.h"

void UAIInvestigationState::OnEnter()
{
	Super::OnEnter();
	AI->SetInvestigation(true);
}

void UAIInvestigationState::OnExit()
{
	Super::OnExit();
	//We are switching to a non investigation state, unregister ourselves as an investigator 
	if (!GetFlaggedState()->GetClass()->IsChildOf(UAIInvestigationState::StaticClass()))
	{
		AI->SetInvestigation(false);
	}
}