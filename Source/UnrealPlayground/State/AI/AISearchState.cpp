#include "AISearchState.h"

void UAISearchState::OnEnter()
{
	Super::OnEnter();
	AI->SetInvestigation(true);
}

void UAISearchState::OnExit()
{
	//We are switching to a state that doesn't involve searching, so we need stop investigating
	if (!GetFlaggedState()->GetClass()->IsChildOf(UAISearchState::StaticClass()))
	{
		AI->SetInvestigation(false);
	}
}