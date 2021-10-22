#include "SVaultState.h"

void USVaultState::OnEnter()
{
	Super::OnEnter();
	OnVaultEnter.Broadcast();
}

void USVaultState::OnExit()
{
	Super::OnExit();
}

void USVaultState::ReceiveVaultAnimComplete()
{
	FlagTransition("Walking");
}
