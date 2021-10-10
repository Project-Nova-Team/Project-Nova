#include "AISearchState.h"

void UAISearchState::OnEnter()
{
	Super::OnEnter();
	AI->SetInvestigation(true);
}