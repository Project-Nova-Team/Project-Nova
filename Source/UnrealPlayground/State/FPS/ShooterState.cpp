#include "ShooterState.h"

void UShooterState::Initialize(UStateMachine* StateMachine, UObject* ContextObject)
{
	Super::Initialize(StateMachine, ContextObject);
	this->Shooter = CastContext<AShooter>();
}

UDelayedActionManager* UShooterState::GetDelayedActionManager() const
{
	return Shooter->GetWorld()->GetAuthGameMode<AShooterGameMode>()->GetDelayedActionManager();
}