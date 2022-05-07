#include "ShooterStateMachine.h"
#include "Movement/SWalkState.h"
#include "Movement/SRunState.h"
#include "Movement/SCrouchState.h"
#include "Event/SVaultState.h"
#include "Event/SDeathState.h"
#include "Event/SVentState.h"
#include "Event/SLookState.h"
#include "Event/SCutsceneState.h"

void UShooterStateMachine::Initialize(UObject* Context)
{
	Super::Initialize(Context);

	this->AddState<USWalkState>(Context, "Walking");
	this->AddState<USRunState>(Context, "Running");
	this->AddState<USCrouchState>(Context, "Crouching", false);
	this->AddState<USVaultState>(Context, "Vaulting", false);
	this->AddState<USDeathState>(Context, "Death");
	this->AddState<USLookState>(Context, "Look");
	this->AddState<USCutsceneState>(Context, "Cutscene");
	this->AddState<USVentState>(Context, "Venting");

	this->SetInitialState("Walking");
	this->GetActiveState()->OnEnter();
}

void UShooterStateMachine::SetState(const FString NewStateKey)
{
	PreviousState = GetActiveState();
	Super::SetState(NewStateKey);
}