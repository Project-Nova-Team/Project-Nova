#include "AIBase.h"
#include "../Gameplay/HealthComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "../Gameplay/MeleeComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AAIBase::AAIBase()
{
	PrimaryActorTick.bCanEverTick = true;

	MeleeComponent = CreateDefaultSubobject<UMeleeComponent>(TEXT("MeleeComponent"));
	MeleeComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform);
	
	Health = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));

	AttackMoveSpeed = 375;
}

void AAIBase::SetVisible(const bool Value)
{
	const ECollisionEnabled::Type Visibility = Value ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision;
	GetCapsuleComponent()->SetCollisionEnabled(Visibility);
	GetMesh()->SetVisibility(Value);
}

void AAIBase::BeginPlay()
{
	Super::BeginPlay();
	DefaultMoveSpeed = GetCharacterMovement()->MaxWalkSpeed;
}

void AAIBase::SetLifeStatus(const bool bIsAlive)
{
	bIsDead = !bIsAlive;

	if (!bIsAlive)
	{
		GetCapsuleComponent()->SetCollisionProfileName("NoCollision");
	}

	else
	{
		GetCapsuleComponent()->SetCollisionProfileName("Pawn");
		Health->Revive();
	}
}