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
		GetMesh()->SetCollisionProfileName("NoCollision");	
		GetCapsuleComponent()->SetCollisionProfileName("Pawn");		
		GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

		//Hack. our meshes will be better
		GetMesh()->SetRelativeLocation(FVector::ZeroVector);
		GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));
	}
}