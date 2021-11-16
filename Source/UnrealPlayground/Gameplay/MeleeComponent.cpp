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
	OnComponentBeginOverlap.AddDynamic(this, &UMeleeComponent::HitActor);
}

void UMeleeComponent::HitActor(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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
