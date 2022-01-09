#pragma once
#include "CoreMinimal.h"

enum EDelayedActionType { DAT_Delay, DAT_OverTime };

DECLARE_DELEGATE(FActionDelegate);

class FDelayedAction;

struct FDelayedActionHandle
{
public:
	FDelayedActionHandle() { }
	~FDelayedActionHandle() { }

	friend class FDelayedAction;

	/** How far along the current action is on a scale of 0-1*/
	float CurrentActionProgress;

	/** Whether or not the action has finished*/
	uint8 bIsComplete : 1;
	
	/** Returns the action this handle belongs to*/
	FDelayedAction* GetAction();

	//TODO add more stuffz
private:
	/** The action this handle belongs to*/
	FDelayedAction* Action;
};

class FDelayedAction
{
public:
	FDelayedAction();
	~FDelayedAction();

	/** Updates the time state and executes the delay function when necessary*/
	void Tick(const float DeltaTime);
	
	/** Returns whether or not this instance is active*/
	uint8 GetIsActive() const { return bIsActive; }

	FDelayedActionHandle* StartAction(FActionDelegate InDelegate, const float CompletionTime, EDelayedActionType TypeOfDelay);

	void StopAction();

private:

	/**	The type of delay this instance is using*/
	EDelayedActionType DelayType;

	/** Whether or not this instance is currently executing a delayed action*/
	uint8 bIsActive : 1;
	
	/** The current amount of time that has passed during this action cycle*/
	float TimeTracker;

	/** The amount of time that passes until this action is completed*/
	float TimeToComplete;

	/** The function this instance calls*/
	FActionDelegate ActionFunction;

	/** Resets time tracking and activity state so they are ready for a new action*/
	void ClearInstance();

	/** Resets handle values when a new action is started*/
	void ClearHandle();

	/** Container struct providing useful information about this action*/
	FDelayedActionHandle Handle;
};