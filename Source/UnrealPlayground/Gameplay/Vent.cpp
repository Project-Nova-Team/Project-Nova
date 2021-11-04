#include "Vent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "HealthComponent.h"
#include "../ShooterGameMode.h"
#include "../Utility/DelayedActionManager.h"

AVent::AVent()
{
	PrimaryActorTick.bCanEverTick = true;
	Frame = CreateDefaultSubobject<UStaticMeshComponent>("Frame");
	SetRootComponent(Frame);

	Grate = CreateDefaultSubobject<UStaticMeshComponent>("Grate");
	Grate->AttachToComponent(Frame, FAttachmentTransformRules::KeepRelativeTransform);

	GrateTrigger = CreateDefaultSubobject<UBoxComponent>("Stuck Prevention");
	GrateTrigger->AttachToComponent(Frame, FAttachmentTransformRules::KeepRelativeTransform);
	GrateTrigger->SetCollisionProfileName("OverlapOnlyPawn");

	Health = CreateDefaultSubobject<UHealthComponent>("Health");
	
	DisableDuration = 10.f;
}

void AVent::BeginPlay()
{
	Super::BeginPlay();
	Health->OnDeath.AddDynamic(this, &AVent::DisableGrateForDuration);
	OnActorBeginOverlap.AddDynamic(this, &AVent::ActorBeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &AVent::ActorEndOverlap);
}

void AVent::DisableGrateForDuration()
{
	Grate->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Grate->SetVisibility(false);
	bIsDisabled = true;
	bDelayRunning = true;
	GetWorld()->GetAuthGameMode<AShooterGameMode>()->GetDelayedActionManager()->StartDelayedAction(this, &AVent::MaybeReEnableGrate, DisableDuration);
}

void AVent::MaybeReEnableGrate()
{
	bDelayRunning = false;

	if (ShouldEnable())
	{
		ReEnableGrate();
	}
}

void AVent::ReEnableGrate()
{
	Health->Revive();
	Grate->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Grate->SetVisibility(true);
	bIsDisabled = false;
}

void AVent::ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	OverlappedPawns--;

	if (ShouldEnable())
	{
		ReEnableGrate();
	}
}

void AVent::ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	OverlappedPawns++;
}