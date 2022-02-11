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

	void Attack() override;

	void SetCombatComponent(UCombatComponent* NewOwner) override;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Damage")
	float Damage;
};
