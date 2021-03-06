#include "MeleeComponent.h"

UMeleeComponent::UMeleeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetCollisionProfileName("OverlapAllDynamic");
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Damage = 35;
}

void UMeleeComponent::BeginPlay()
{
	Super::BeginPlay();
	IgnoreActorWhenMoving(GetOwner(), true);
	OnComponentBeginOverlap.AddDynamic(this, &UMeleeComponent::HitActor);
}

void UMeleeComponent::HitActor(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UClass* const OtherClass = OtherActor->GetClass();

	if (ClassWhiteList.Contains(OtherClass) && !HitCollection.Contains(OtherActor))
	{	
		HitCollection.Emplace(OtherActor);
		OtherActor->TakeDamage(Damage, FDamageEvent(), nullptr, GetOwner());

		if (HitCollection.Num() == 1)
		{
			OnFirstHit.Broadcast(OtherActor, OtherClass);
		}
	}
}

void UMeleeComponent::StartAttack()
{
	HitCollection.Reset();
	SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void UMeleeComponent::EndAttack()
{
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
}