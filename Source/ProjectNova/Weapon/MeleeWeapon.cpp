#include "MeleeWeapon.h"
#include "Weapon.h"

AMeleeWeapon::AMeleeWeapon()
{
	BaseDamage = 50.f;
}

void AMeleeWeapon::StartAttack()
{
	OnWeaponAttack.Broadcast();
}