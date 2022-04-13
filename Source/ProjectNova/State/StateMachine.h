#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "StateMachine.generated.h"

class UState;

UCLASS()
class PROJECTNOVA_API UStateMachine : public UObject
{
	GENERATED_BODY()
	
public:
	UStateMachine() { }
	~UStateMachine() { }

	/** Gets the active state*/
	UState* GetActiveState() const { return ActiveState; }

	/** Gets state at key value*/
	UState* GetStateAtKey(const FString& Key) const { return States[Key]; }

	/** Templated version of GetStateAtKey that peforms the cast*/
	template<class T>
	T* GetStateAtKey(const FString& Key) const { return Cast<T>(States[Key]); }

	/** Gets the collection of states this StateMachine has*/
	TMap<FString, UState*> GetStates() const { return States; }

	/** 
	* Call this during BeginPlay. Override this and add any desired states using AddState()
	* 
	* @param	Context					The Object that will be given to the States do game logic on
	*/
	virtual void Initialize(UObject* Context);

	/** Executes the active state's tick function and handles transitions after the state logic completes*/
	void Tick(const float DeltaTime);

	/**
	* Removes a given state from the States map and flags it for garbage collection if it is not the currently active state. Use sparingly
	* 
	* @param	Key						The key of the state in this state machine's map that is getting removed
	*/
	void RemoveState(const FString Key);

	/**
	 * Immediatly transitions to the given state instead of waiting for the next tick cycle
	 * State will NOT change if the state already has a flagged transition with a higher weight
	 *
	 * @param	Key						The key of the state in this state machine's map
	 * @param	Weight				Priority value for this change, higher values have higher priority
	 */
	void SetStateImmediate(const FString Key, const uint8 Weight);

	/** 
	 * Immediatly transitions to the given state instead of waiting for the next tick cycle
	 * State will NOT change if the state already has a flagged transition with a higher weight
	 * 
	 * @param	State				Pointer to a state in this state machine's map
	 * @param	Weight				Priority value for this change, higher values have higher priority
	 */
	void SetStateImmediate(UState* State, const uint8 Weight);

protected:
	
	/**
	* Set the initial state of the State Machine. Call this after you've Added the desired state
	* 
	* @param	Key						The key of the state in this state machine's map
	*/
	void SetInitialState(const FString Key) { ActiveState = States[Key]; }

	/**
	* Set the initial state of the State Machine. Call this after you've Added the desired state
	* 
	* @param	State					Pointer to a state in this state machine's map
	*/
	void SetInitialState(UState* State) { ActiveState = State; }

	/** Internally executes state transition logic*/
	virtual void SetState(const FString NewStateKey);

	/** Internally executes state transition logic*/
	virtual void SetState(UState* NewState);

	/**
	* Creates a state instance of the supplied class type and adds it to the States map
	* 
	* @param	Context					The object states perform logic on
	* @param	Key						The value that will be used to access this state
	* @param	bIsActive				Whether or not this state should be activated when added to this machine
	*/
	template <class TStateType>
	void AddState(UObject* Context, const FString Key, const bool bIsActive = true)
	{
		//TODO Find a way to generate a compile error instead of runtime error when type isn't subclass of UState
		if (false)
		{
			FString LogMessage = TEXT("An incompatible object with key '") + Key + TEXT("' was added to a StateMachine map. All objects added to this map must inherit from UState.");
			UE_LOG(LogTemp, Fatal, TEXT("%s"), *LogMessage);
		}
		
		this->AddState(Context, Key, NewObject<TStateType>(), bIsActive);
	}


	/**
	* Adds the supplied state reference to the State map
	* 
	* @param	Context					The object states perform logic on
	* @param	Key						The value that will be used to access this state
	* @param	NewState				A reference to a UState instance that should be added to this state machine
	* @param	bIsActive				Whether or not this state should be activated when added to this state machine
	*/
	void AddState(UObject* Context, const FString Key, UState* NewState, const bool bIsActive = true);

private:
	/** The currently active state that will have its Tick function*/
	UState* ActiveState;

	/** This is the collection of states that the state machine currently has access to*/
	TMap<FString, UState*> States;
};
