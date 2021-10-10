#include "AIBase.h"
#include "../Gameplay/HealthComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "../Gameplay/MeleeComponent.h"
#include "Components/CapsuleComponent.h"

AAIBase::AAIBase()
{
	PrimaryActorTick.bCanEverTick = true;

	MeleeComponent = CreateDefaultSubobject<UMeleeComponent>(TEXT("MeleeComponent"));
	MeleeComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform);
	
	Health = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));
}

void AAIBase::SetLifeStatus(const bool bIsAlive)
{
	if (!bIsAlive)
	{
		GetMesh()->SetAnimationMode(EAnimationMode::AnimationCustomMode);
		GetCapsuleComponent()->SetCollisionProfileName("NoCollision");
		GetMesh()->SetCollisionProfileName("Ragdoll");	
		GetMesh()->SetSimulatePhysics(true);
	}

	else
	{
		GetMesh()->SetSimulatePhysics(false);
		GetMesh()->SetCollisionProfileName("HitScan");	
		GetCapsuleComponent()->SetCollisionProfileName("Pawn");
		GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	}
}