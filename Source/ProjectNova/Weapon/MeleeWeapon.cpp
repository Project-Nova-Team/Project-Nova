#include "MeleeWeapon.h"
#include "Weapon.h"

AMeleeWeapon::AMeleeWeapon()
{
	Damage = 20.f;
}

void AMeleeWeapon::Attack()
{
	ReceiveAttack();
}

void AMeleeWeapon::SetCombatComponent(UCombatComponent* NewOwner)
{
	Super::SetCombatComponent(NewOwner);

	if (NewOwner != nullptr)
	{
		Mesh->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));
	}

	else
	{
		Mesh->SetCollisionProfileName(TEXT("Ragdoll"));
	}
}