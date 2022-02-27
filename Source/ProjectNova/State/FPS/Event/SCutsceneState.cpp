#include "SCutsceneState.h"
#include "../../../Weapon/CombatComponent.h"

void USCutsceneState::OnEnter()
{
	Shooter->SetInputEnabled(false);

	if (AWeapon* Weapon = Shooter->GetCombat()->GetHeldWeapon())
	{
		Weapon->GetRootComponent()->SetVisibility(false);
	}
}


void USCutsceneState::OnExit()
{
	Shooter->SetInputEnabled(true);

	if (AWeapon* Weapon = Shooter->GetCombat()->GetHeldWeapon())
	{
		Weapon->GetRootComponent()->SetVisibility(true);
	}
}