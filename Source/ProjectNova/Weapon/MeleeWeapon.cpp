#include "MeleeWeapon.h"
#include "Weapon.h"
#include "Components/SphereComponent.h"
#include "DamageTypeMelee.h"
#include "CombatComponent.h"

AMeleeWeapon::AMeleeWeapon()
{
	Damage = 20.f;
	
	if (!OnActorBeginOverlap.Contains(this, TEXT("ReceiveOverlap")))
	{
		OnActorBeginOverlap.AddDynamic(this, &AMeleeWeapon::ReceiveOverlap);
	}

	HitCollider = CreateDefaultSubobject<USphereComponent>(TEXT("Damage Trigger"));
	HitCollider->AttachToComponent(Mesh, FAttachmentTransformRules::KeepRelativeTransform);
	HitCollider->SetCollisionProfileName(TEXT("OverlapAll"));
	HitCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AMeleeWeapon::Attack()
{
	ReceiveAttack();
}

void AMeleeWeapon::SetCombatComponent(UCombatComponent* NewOwner)
{
	Super::SetCombatComponent(NewOwner);

	if (NewOwner != nullptr)
	{
		OwnerActor = TraceOrigin->GetOwner();
		OwnerAnimInstance = NewOwner->GetAttachmentMesh()->GetAnimInstance();
	}

	else
	{
		OwnerAnimInstance = nullptr;
		OwnerActor = nullptr;
		DisableHitCollider();
	}
}

void AMeleeWeapon::ReceiveOverlap(AActor* Self, AActor* OtherActor)
{
	if (OtherActor != OwnerActor && !HitActors.Contains(OtherActor))
	{
		OtherActor->TakeDamage(Damage, FDamageEvent(UDamageTypeMelee::StaticClass()), OwnerActor->GetInstigatorController(), OwnerActor);

		ReceiveHitActor(OtherActor);
		HitActors.Add(OtherActor);	
	}
}

void AMeleeWeapon::EnableHitCollider()
{
	HitCollider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AMeleeWeapon::DisableHitCollider()
{
	HitCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitActors.Empty();
}