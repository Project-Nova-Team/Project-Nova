#include "AIStateMachine.h"
#include "AIPatrolState.h"
#include "AIAttackState.h"
#include "AIAgitatedState.h"
#include "AIInvestigateState.h"
#include "AITrackState.h"
#include "AIFollowState.h"
#include "AIDeathState.h"

void UAIStateMachine::Initialize(UObject* Context)
{
	Super::Initialize(Context);

	this->AddState<UAIPatrolState>(Context, "Patrol");
	this->AddState<UAIAttackState>(Context, "Attack");
	this->AddState<UAIAgitatedState>(Context, "Agitated");
	this->AddState<UAIInvestigateState>(Context, "Investigate");
	this->AddState<UAITrackState>(Context, "Track");
	this->AddState<UAIFollowState>(Context, "Follow");
	this->AddState<UAIDeathState>(Context, "Dead");

	this->SetInitialState("Patrol");	
}

void UAIStateMachine::PostInitLoadTrees(const TMap<FString, UBehaviorTree*>& TreeMap)
{
	TArray<FString> Keys;
	const int32 Num = TreeMap.Num();
	Keys.Reset();
	Keys.Reserve(Num);
	TreeMap.GetKeys(Keys);

	//Breaks if you mess up. Be careful
	for (FString S : Keys)
	{
		Cast<UAIState>(GetStateAtKey(S))->SetTree(TreeMap[S]);
	}

	this->GetActiveState()->OnEnter();
}

uint8 UAIStateMachine::GetActiveWeight()
{
	return Cast<UAIState>(GetActiveState())->Weight;
}

uint8 UAIStateMachine::GetWeightAtKey(const FString& Key)
{
	return Cast<UAIState>(GetStateAtKey(Key))->Weight;
}