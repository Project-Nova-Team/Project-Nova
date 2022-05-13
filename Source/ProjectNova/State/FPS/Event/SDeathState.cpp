#include "SDeathState.h"

void USDeathState::OnEnter()
{
	Shooter->GetInput()->bIsMoving = false;
	//@todo
}
