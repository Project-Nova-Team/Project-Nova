#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "MeleeComponent.generated.h"

UCLASS()
class UNREALPLAYGROUND_API UMeleeComponent : public USphereComponent
{
	GENERATED_BODY()

public:
	UMeleeComponent();

	UFUNCTION(BlueprintCallable, Category = "Melee")
	void StartAttack();

	UFUNCTION(BlueprintCallable, Category = "Melee")
	void EndAttack();

	void SetDamage(const float Value) { Damage = Value; }

protected:
	void BeginPlay() override;
private:

	/** How much damage dealt with each attack*/
	UPROPERTY(EditAnywhere, Category = "Melee")
	float Damage;

	/** 
	 * This component will only hit actors of these subclasses.
	 * note: this implementation requires the class to be EXACTLY of the type!
	 */
	UPROPERTY(EditAnywhere, Category = "Melee")
	TArray<UClass*> ClassWhiteList;

	/** Current collection of actors hit by this component. This will be cleared when an attack starts*/
	TArray<AActor*> HitCollection;
	
	UFUNCTION()
	/** Event function bound to OnComponentBeginOverlap, deals damage to the actor hit if it exists in ClassWhiteList*/
	void HitActor(AActor* OverlappedActor, AActor* OtherActor);
};
