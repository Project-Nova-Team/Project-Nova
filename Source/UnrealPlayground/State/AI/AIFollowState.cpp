#include "AIFollowState.h"
void UAIFollowState::Tick(const float DeltaTime)
{
	AI->SetMoveLocation(AI->GetTarget()->GetActorLocation());
	//We've followed the target long enough, switch to investigation mode
	if (AI->GetWorld()->GetTimeSeconds() > AI->GetLastFollowTime() + AI->FollowDuration)
	{
		FlagTransition("Investigate");
	}
}

void UAIFollowState::Initialize(UStateMachine* StateMachine, UObject* ContextObject)
{
	Super::Initialize(StateMachine, ContextObject);
	Weight = 5;
}

void UAIFollowState::OnEnter()
{
	Super::OnEnter();
	AI->SetSearchParameters(true);
}