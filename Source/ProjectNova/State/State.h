#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../ShooterGameMode.h"
#include "State.generated.h"

class UStateMachine;

/*A State is an object that performs game logic on a Context object.*/
UCLASS()
class PROJECTNOVA_API UState : public UObject
{
	GENERATED_BODY()

public:
	UState() { }
	~UState() {	}

	/** Called when this state gets added to a state machine. Set any initial property values here*/
	virtual void Initialize(UStateMachine* StateMachine, UObject* ContextObject);

	/** Called when this state's StateMachine enters this state*/
	virtual void OnEnter() { }

	/** Called when this state's StateMachine exits this state*/
	virtual void OnExit() { }

	/** 
	* Flags this state to be transitioned from
	* 
	* @param		Key				The key being transitioned to
	* @param		Weight			Priority value for this tick, higher values will have higher priority
	* @returns		True			if the flag was successful
	*/
	virtual bool FlagTransition(const FString Key, const uint8 Weight = 1);

	/**
	* Flags this state to be transitioned from
	*
	* @param		State			The state being transitioned to
	* @param		Weight			Priority value for this tick, higher values will have higher priority
	* @returns		True			if the flag was successful
	*/
	virtual bool FlagTransition(UState* State, const uint8 Weight = 1);

	/** Resets flag, key, and weight values. Called once a transition has been approved*/
	virtual void ClearTransitionFlags();

	/** Called every game Tick this state is set as a StateMachine's active state*/
	virtual void Tick(const float DeltaTime) { }

	/** Gets the StateMachine this state belongs to*/
	UStateMachine* GetMachine() const { return Machine; }

	/** Gets the state's context object*/
	UObject* GetContext() const { return Context; }

	/** Returns whether or not this state is requesting a transition*/
	bool GetTransitionFlagged() const { return bTransitionFlagged; }

	/** Returns whether or not this state is active and can be queued*/
	bool GetIsActive() const { return bIsActive; }

	/** Set whether or not this state can be queued*/
	void SetIsActive(const bool Value) { bIsActive = Value; }

	/** Returns the Key of the state currently requested as a transition*/
	FString GetFlaggedKey() const { return FlaggedKey; }

	UState* GetFlaggedState() const { return FlaggedState; }

protected:
	
	/** Sets the Key value that will be used for the next state transition, if one exists*/
	void SetFlaggedKey(const FString Value) { FlaggedKey = Value; }

	/** Sets the State value that will be used for the next state transition, if one exists*/
	void SetFlaggedState(UState* Value) { FlaggedState = Value; }

	/** Sets the Weight value that is used when determining state transition*/
	void SetCurrentWeight(const uint8 Value) { CurrentWeight = Value; }

	/**
	* Casts the context object to the desired type. Use this to get access to game property values
	* Should this be marked as inline/FORCEDINLINE?
	*/
	template<class TContextType>
	TContextType* CastContext()
	{
		return Cast<TContextType>(this->Context);
	}

private:

	/** The StateMachine this state belongs to*/
	UStateMachine* Machine;

	/** The context object this state performs logic on*/
	UObject* Context;

	/** Whether or not this state is currently active and can be queued*/
	bool bIsActive;

	/** Whether or not this state is requesting to be transitioned from*/
	bool bTransitionFlagged;

	/** The key value of the state this state is requesting to transition to*/
	FString FlaggedKey;

	/** A pointer to the state this state is requesting to transition to*/
	UState* FlaggedState;

	/** Used to determine if a different state transition should be flagged this Tick*/
	uint8 CurrentWeight;
};
