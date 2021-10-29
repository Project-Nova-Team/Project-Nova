#include "MeleeComponent.h"

UMeleeComponent::UMeleeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetCollisionProfileName("NoCollision");
	
	Damage = 35;
}

void UMeleeComponent::BeginPlay()
{
	Super::BeginPlay();
	IgnoreActorWhenMoving(GetOwner(), true);
	GetOwner()->OnActorBeginOverlap.AddDynamic(this, &UMeleeComponent::HitActor);
}

void UMeleeComponent::HitActor(AActor* OverlappedActor, AActor* OtherActor)
{
	UClass* const OtherClass = OtherActor->GetClass();

	if (ClassWhiteList.Contains(OtherClass) && !HitCollection.Contains(OtherActor))
	{	
		HitCollection.Emplace(OtherActor);
		OtherActor->TakeDamage(Damage, FDamageEvent(), nullptr, GetOwner());
	}
}

void UMeleeComponent::StartAttack()
{
	HitCollection.Reset();
	SetCollisionProfileName("OverlapOnlyPawn");
}

void UMeleeComponent::EndAttack()
{
	SetCollisionProfileName("NoCollision");
}
