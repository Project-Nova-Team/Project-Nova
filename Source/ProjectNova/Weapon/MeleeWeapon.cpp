#include "MeleeWeapon.h"
#include "Weapon.h"
#include "Components/SphereComponent.h"

AMeleeWeapon::AMeleeWeapon()
{
	Damage = 20.f;
	
	if (!OnActorBeginOverlap.Contains(this, TEXT("ReciveOverlap")))
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

		//Leaves area to be improved
		if (USkeletalMeshComponent* OwnerMesh = OwnerActor->FindComponentByClass<USkeletalMeshComponent>())
		{
			OwnerAnimInstance = OwnerMesh->AnimScriptInstance;
		}
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
		OtherActor->TakeDamage(Damage, FDamageEvent(), OwnerActor->GetInstigatorController(), OwnerActor);

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