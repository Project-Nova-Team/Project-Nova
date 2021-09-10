#include "ShooterState.h"

void UShooterState::Initialize(UStateMachine* StateMachine, UObject* ContextObject)
{
	Super::Initialize(StateMachine, ContextObject);
	this->Shooter = CastContext<AShooter>();
}