#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHealthEvent);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTNOVA_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UHealthComponent();

	UFUNCTION(BlueprintCallable, Category = "Health")
	void Heal(const float HealAmount);

	/** Revives the actor to maximum health*/
	void Revive();

	/** Revives the actor to the specified health*/
	UFUNCTION(BlueprintCallable, Category = "Health")
	void Revive(const float ReviveHealth);

	/** Whether or not the current health is equal to the max health*/
	UPROPERTY(BlueprintReadOnly, Category = "Health")
	uint8 bIsFullHealth : 1;

	/** Delegate executed when this components actor reaches 0 health*/
	UPROPERTY(BlueprintAssignable)
	FHealthEvent OnDeath;

	/** Delegate executed when this components actor reaches 0 health*/
	UPROPERTY(BlueprintAssignable)
	FHealthEvent OnDamaged;

	/** Delegate executed anytime this component's actor revives*/
	UPROPERTY(BlueprintAssignable)
	FHealthEvent OnRevive;

	void SetStartingHealth(float Amount);

	float GetStartingHealth() const { return StartingHealth; }

protected:
	virtual void BeginPlay() override;

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
