#include "AIBase.h"
#include "../Gameplay/HealthComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"


AAIBase::AAIBase()
{
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(GetMesh(), TEXT("WeaponSocket"));

	DamageTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("DamageTrigger"));
	DamageTrigger->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform);
	DamageTrigger->SetCollisionProfileName("OverlapOnlyPawn");
	DamageTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	Health = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));
}

void AAIBase::BeginPlay()
{
	Super::BeginPlay();
	OnTakeAnyDamage.AddDynamic(this, &AAIBase::OnTookDamage);
	DamageTrigger->OnComponentBeginOverlap.AddDynamic(this, &AAIBase::OnAttackHit);
}

void AAIBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

void AAIBase::OnDeath()
{
	GetCapsuleComponent()->SetCollisionProfileName("NoCollision");
	GetMesh()->SetCollisionProfileName("Ragdoll");
	GetMesh()->SetAnimInstanceClass(nullptr);
	GetMesh()->SetSimulatePhysics(true);
}

void AAIBase::OnTookDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	const FVector ActorLoc = GetActorLocation();
	//UAISense_Damage::ReportDamageEvent(GetWorld(), this, Player, Damage, ActorLoc, ActorLoc);
}