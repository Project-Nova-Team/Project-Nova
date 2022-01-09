#include "DelayedAction.h"

FDelayedAction* FDelayedActionHandle::GetAction()
{
	return Action;
}

FDelayedAction::FDelayedAction()
{
	bIsActive = false;
	TimeToComplete = 0;
	TimeTracker = 0;
	DelayType = DAT_Delay;

	Handle = FDelayedActionHandle();
}

FDelayedAction::~FDelayedAction() { }

void FDelayedAction::Tick(const float DeltaTime)
{
	if (bIsActive)
	{
		TimeTracker += DeltaTime;
		Handle.CurrentActionProgress = FMath::Clamp(TimeTracker / TimeToComplete, 0.f, 1.f);

		if (DelayType == DAT_OverTime)
			ActionFunction.ExecuteIfBound();
			
		if (TimeTracker > TimeToComplete)
		{
			Handle.bIsComplete = true;

			if (DelayType == DAT_Delay)
				ActionFunction.ExecuteIfBound();	

			ClearInstance();
		}
	}
}

FDelayedActionHandle* FDelayedAction::StartAction(FActionDelegate InDelegate, const float CompletionTime, EDelayedActionType TypeOfDelay)
{
	ActionFunction = InDelegate;
	TimeToComplete = CompletionTime;
	DelayType = TypeOfDelay;
	bIsActive = true;
	Handle.Action = this;

	ClearHandle();
	return &Handle;
}

void FDelayedAction::StopAction()
{
	ClearInstance();
	ClearHandle();
}

void FDelayedAction::ClearInstance()
{
	bIsActive = false;
	ActionFunction = nullptr;
	Handle.Action = nullptr;
	TimeTracker = 0;
}

void FDelayedAction::ClearHandle()
{
	Handle.bIsComplete = false;
	Handle.CurrentActionProgress = 0;
}