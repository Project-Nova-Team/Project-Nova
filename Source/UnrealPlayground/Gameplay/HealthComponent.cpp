#include "HealthComponent.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	MaxHealth = 100.f;
	StartingHealth = MaxHealth;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	Health = StartingHealth;
	bIsFullHealth = Health == MaxHealth;

	AActor* Owner = GetOwner();

	if (Owner != nullptr)
	{
		//Intellisense is lying, this is fine
		Owner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::TakeDamage);
	}
}

void UHealthComponent::TakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.f || bIsInvincible)
	{
		return;
	}

	Health -= Damage;
	bIsFullHealth = false;

	//Health dropped below zero, broadcast death
	if (Health <= 0)
	{
		Health = 0;
		OnDeath.Broadcast();
		bIsInvincible = true;
	}
}

void UHealthComponent::Heal(const float HealAmount)
{
	if (HealAmount <= 0.f)
	{
		return;
	}

	Health += HealAmount;
	
	if (Health >= MaxHealth)
	{
		Health = MaxHealth;
		bIsFullHealth = true;
	}
}

void UHealthComponent::Revive()
{
	Heal(MaxHealth);
	bIsInvincible = false;
}

