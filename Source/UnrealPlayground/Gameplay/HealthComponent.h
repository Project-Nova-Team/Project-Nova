#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHealthEvent);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREALPLAYGROUND_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UHealthComponent();

	UFUNCTION()
	void Heal(const float HealAmount);

	/** Whether or not the current health is equal to the max health*/
	UPROPERTY(BlueprintReadOnly, Category = "Health")
		uint8 bIsFullHealth : 1;

protected:
	virtual void BeginPlay() override;

	/** Event that is fired when taking damage that would leave us below 0 health*/
	UPROPERTY(BlueprintAssignable)
	FHealthEvent OnDeath;
	
	/** The maximum amount of health that can be had at any given time*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth;

	/** The current health will be set to this value in BeginPlay*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float StartingHealth;

	/** The actual current health*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	float Health;

	/** Whether or not the owner is currently able to take damage*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	uint8 bIsInvincible : 1;

	UFUNCTION()
	void TakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
};
