#include "StateMachine.h"
#include "State.h"

void UStateMachine::Initialize(UObject* Context)
{

}

void UStateMachine::Tick(const float DeltaTime)
{
	ActiveState->Tick(DeltaTime);

	if (ActiveState->GetTransitionFlagged())
	{
		if (!ActiveState->GetFlaggedKey().IsEmpty())
			SetState(ActiveState->GetFlaggedKey());
		else if (ActiveState->GetFlaggedState() != nullptr)
			SetState(ActiveState->GetFlaggedState());
	}
}

void UStateMachine::AddState(UObject* Context, const FString Key, UState* NewState, const bool bIsActive)
{
	NewState->Initialize(this, Context);
	NewState->SetIsActive(bIsActive);
	States.Add(Key, NewState);
}

void UStateMachine::RemoveState(const FString& Key)
{
	if (States[Key] != nullptr)
	{
		if (ActiveState == States[Key])
		{
			FString LogMessage = TEXT("State `" + States[Key]->GetFName().GetPlainNameString() + "` with Key `" + Key + "` was attempted to be removed while it was the active state.");
			UE_LOG(LogTemp, Error, TEXT("%s"), *LogMessage);
			return;
		}

		UState* State = States[Key];
		States.Remove(Key);
		State->ConditionalBeginDestroy();
	}
}

void UStateMachine::SetState(const FString& NewStateKey)
{
	ActiveState->OnExit();
	ActiveState->ClearTransitionFlags();
	ActiveState = States[NewStateKey];
	ActiveState->OnEnter();
}

void UStateMachine::SetState(UState* NewState)
{	
	ActiveState->OnExit();
	ActiveState->ClearTransitionFlags();
	ActiveState = NewState;
	ActiveState->OnEnter();
}

void UStateMachine::SetStateImmediate(const FString& NewStateKey, const uint8 Weight)
{
	if (ActiveState->FlagTransition(NewStateKey, Weight))
	{
		SetState(ActiveState->GetFlaggedState());
	}
}

void UStateMachine::SetStateImmediate(UState* NewState, const uint8 Weight)
{
	if (ActiveState->FlagTransition(NewState, Weight))
	{
		SetState(ActiveState->GetFlaggedState());
	}
}