#include "AIBase.h"
#include "../Gameplay/HealthComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"

AAIBase::AAIBase()
{
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(GetMesh(), TEXT("WeaponSocket"));

	DamageTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("DamageTrigger"));
	DamageTrigger->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform);
	DamageTrigger->SetCollisionProfileName("OverlapOnlyPawn");
	DamageTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	Instruction = CreateDefaultSubobject<UInstructionComponent>(TEXT("Instruction"));
	Health = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));
}

void AAIBase::BeginPlay()
{
	Super::BeginPlay();
	DamageTrigger->OnComponentBeginOverlap.AddDynamic(this, &AAIBase::OnAttackHit);
}

void AAIBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// These ticks imply some level of bad separation of concern but is more performent 
	// since we dont have manage tick on the instruction component itself

	//Probably safe to remove second condition if we manage state search mode properly
	if (Instruction->GetSearchMode() == ESearchMode::Follow && Instruction->GetState() == EInstructionState::Search)
	{
		Instruction->TickFollow(GetWorld()->GetTimeSeconds());
	}

	if (Instruction->GetState() == EInstructionState::Attack)
	{
		Instruction->TickAttack(GetWorld()->GetTimeSeconds());
	}
}

void AAIBase::OnAttackBegin()
{
	DamageTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AAIBase::OnAttackEnd()
{
	DamageTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	bHasHitPlayer = false;
}

void AAIBase::OnAttackHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bHasHitPlayer && OtherActor)
	{
		bHasHitPlayer = true;
	}
}