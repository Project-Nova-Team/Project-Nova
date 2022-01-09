#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "MeleeWeapon.generated.h"

UCLASS()
class PROJECTNOVA_API AMeleeWeapon : public AWeapon
{
	GENERATED_BODY()
	
public:	
	AMeleeWeapon();

	void StartAttack() override;
};
