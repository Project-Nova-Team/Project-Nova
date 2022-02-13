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

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Damage")
	float Damage;

	/** Melee weapons hold a reference to their owner's animation instance. This allows them to execute attack montages on attack*/
	UPROPERTY(BlueprintReadOnly)
	UAnimInstance* OwnerAnimInstance;

	/** Component responsible for hit collisions*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collider")
	class USphereComponent* HitCollider;

public:

	void Attack() override;
	void SetCombatComponent(UCombatComponent* NewOwner) override;

	UFUNCTION(BlueprintCallable)
	void EnableHitCollider();

	UFUNCTION(BlueprintCallable)
	void DisableHitCollider();

protected:
	
	/** Executed when this melee weapons overlaps with an actor that is not the holder*/
	UFUNCTION()
	void ReceiveOverlap(AActor* Self, AActor* OtherActor);

	/** Executed upon hitting an object. Useful for playing audio events*/
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Hit Actor"))
	void ReceiveHitActor(AActor * Actor);

private:

	/** Collection of actors hit by the current swing. Ensures the same actor is not hit multiple times by one attack*/
	TArray<AActor*> HitActors;

	/** Actor that holds this object, ensures we don't accidently hit ourselves*/
	AActor* OwnerActor;
};
