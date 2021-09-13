#include "AIBase.h"
#include "InstructionComponent.h"
#include "../Weapon/CombatComponent.h"
#include "../Gameplay/HealthComponent.h"
#include "Components/SkeletalMeshComponent.h"

AAIBase::AAIBase()
{
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform);

	TraceOrigin = CreateDefaultSubobject<USceneComponent>(TEXT("TraceOrigin"));
	TraceOrigin->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform);

	Instruction = CreateDefaultSubobject<UInstructionComponent>(TEXT("Instruction"));

	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat"));
	Combat->SetUpConstruction(TraceOrigin, WeaponMesh);

	Health = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));
}

void AAIBase::BeginPlay()
{
	Super::BeginPlay();
	Combat->InitializeInput(&Input);
}

void AAIBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}