#include "AISearchState.h"

void UAISearchState::OnExit()
{
	if (!GetFlaggedState()->GetClass()->IsChildOf(UAISearchState::StaticClass()))
	{
		AI->SetStartingSearch(true);
	}
}