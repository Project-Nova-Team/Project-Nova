#include "DelayedActionManager.h"

void UDelayedActionManager::Initialize()
{
	for (int i = 0; i < 10; i++)
	{
		Actions.Add(TUniquePtr<FDelayedAction>(new FDelayedAction()));
	}
}

void UDelayedActionManager::TickRunningActions(const float DeltaTime)
{
	for (int i = 0; i < Actions.Num(); i++)
	{
		Actions[i]->Tick(DeltaTime);
	}
}

void UDelayedActionManager::StopDelayedAction(FDelayedActionHandle* ActionHandle)
{
	ActionHandle->GetAction()->StopAction();
}

void UDelayedActionManager::StopDelayedAction(FDelayedAction* Action)
{
	Action->StopAction();
}

int32 UDelayedActionManager::GetAvailableAction()
{
	for (int32 i = 0; i < Actions.Num(); i++)
	{
		if (!Actions[i]->GetIsActive())
			return i;
	}

	AddNewDelayedActionInstance();

	return Actions.Num();
}

void UDelayedActionManager::AddNewDelayedActionInstance()
{
	Actions.Add(TUniquePtr<FDelayedAction>(new FDelayedAction()));
}

