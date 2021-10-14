#include "State.h"
#include "StateMachine.h"

void UState::Initialize(UStateMachine* StateMachine, UObject* ContextObject)
{
	Machine = StateMachine;
	Context = ContextObject;
	this->AddToRoot();
}

void UState::ClearTransitionFlags()
{
	if (bTransitionFlagged)
	{
		bTransitionFlagged = false;
		SetCurrentWeight(0);
		SetFlaggedKey("");
		SetFlaggedState(nullptr);
	}	
}

void UState::FlagTransition(const FString Key, const uint8 Weight)
{
	/*This horizontal evaluation may indicate there is an architectural issue in the StateMachine's design*/
	UState* TransitionState = GetMachine()->GetStates()[Key];

	if (TransitionState != nullptr)
	{
		if (TransitionState != this && TransitionState->GetIsActive() && Weight > CurrentWeight)
		{
			bTransitionFlagged = true;
			SetFlaggedKey(Key);
			SetFlaggedState(TransitionState);
			SetCurrentWeight(Weight);
		}
	}

	else
	{
		FString LogMessage = TEXT("Transition to state at Key `" + Key + "` returned null. Did you mistype the state Key?");
		UE_LOG(LogTemp, Error, TEXT("%s"), *LogMessage);
	}														
}

void UState::FlagTransition(UState* State, const uint8 Weight)
{
	if (State != nullptr)
	{
		if (State != this && State->GetIsActive() && Weight > CurrentWeight)
		{
			bTransitionFlagged = true;
			SetFlaggedState(State);
			SetCurrentWeight(Weight);
		}
	}

	else
	{
		FString LogMessage = TEXT("Transition to state `" + State->GetFName().GetPlainNameString() + "` returned null. Did you not add the state to the state machine?");
		UE_LOG(LogTemp, Error, TEXT("%s"), *LogMessage);
	}
}