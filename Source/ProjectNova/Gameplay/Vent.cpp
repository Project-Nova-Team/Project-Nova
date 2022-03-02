#include "Vent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "HealthComponent.h"
#include "../Player/Shooter.h"
#include "../State/State.h"
#include "../ShooterGameMode.h"
#include "../State/FPS/ShooterStateMachine.h"
#include "../Utility/DelayedActionManager.h"

AVent::AVent()
{
	PrimaryActorTick.bCanEverTick = false;
	
	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
	SetRootComponent(Spline);

	LeftFrame = CreateDefaultSubobject<UStaticMeshComponent>("LeftFrame");
	LeftFrame->AttachToComponent(Spline, FAttachmentTransformRules::KeepRelativeTransform);

	RightFrame = CreateDefaultSubobject<UStaticMeshComponent>("RightFrame");
	RightFrame->AttachToComponent(Spline, FAttachmentTransformRules::KeepRelativeTransform);

	LeftGrate = CreateDefaultSubobject<UStaticMeshComponent>("LeftGrate");
	LeftGrate->AttachToComponent(LeftFrame, FAttachmentTransformRules::KeepRelativeTransform);

	RightGrate = CreateDefaultSubobject<UStaticMeshComponent>("RightGrate");
	RightGrate->AttachToComponent(RightFrame, FAttachmentTransformRules::KeepRelativeTransform);

	LeftGrateTrigger = CreateDefaultSubobject<UBoxComponent>("Left Stuck Prevention");
	LeftGrateTrigger->AttachToComponent(LeftFrame, FAttachmentTransformRules::KeepRelativeTransform);
	RightGrateTrigger = CreateDefaultSubobject<UBoxComponent>("Right Stuck Prevention");
	RightGrateTrigger->AttachToComponent(RightFrame, FAttachmentTransformRules::KeepRelativeTransform);

	LeftGrateTrigger->SetCollisionProfileName("OverlapOnlyPawn");
	RightGrateTrigger->SetCollisionProfileName("OverlapOnlyPawn");

	Health = CreateDefaultSubobject<UHealthComponent>("Health");
	
	DisableDuration = 10.f;
}

void AVent::InteractionEvent(APawn* EventSender)
{
	if (AShooter* Shooter = Cast<AShooter>(EventSender))
	{
		Shooter->GetStateMachine()->SetState("Venting");
		DisableGrateForDuration();
	}
}

void AVent::BeginPlay()
{
	bCanInteract = true; // temp

	Super::BeginPlay();
	Health->OnDeath.AddDynamic(this, &AVent::DisableGrateForDuration);
	LeftGrateTrigger->OnComponentBeginOverlap.AddDynamic(this, &AVent::ComponentBeginOverlap);
	LeftGrateTrigger->OnComponentEndOverlap.AddDynamic(this, &AVent::ComponentEndOverlap);
	RightGrateTrigger->OnComponentBeginOverlap.AddDynamic(this, &AVent::ComponentBeginOverlap);
	RightGrateTrigger->OnComponentEndOverlap.AddDynamic(this, &AVent::ComponentEndOverlap);
}

void AVent::DisableGrateForDuration()
{
	bCanInteract = true; // can interact while grate is down
	LeftGrate->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightGrate->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftGrate->SetVisibility(false);
	RightGrate->SetVisibility(false);
	OnVentDisabled.Broadcast();
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
	LeftGrate->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	RightGrate->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	LeftGrate->SetVisibility(true);
	RightGrate->SetVisibility(true);
	bIsDisabled = false;
	OnVentEnabled.Broadcast();
}

void AVent::ComponentEndOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	OverlappedPawns--;
	if (Cast<AShooter>(OtherActor))
	{
		if (ShouldEnable())
		{
			ReEnableGrate();
		}

		if (Cast<UBoxComponent>(HitComp) == LeftGrateTrigger)
		{
			bIsOverlappingLeftTrigger = false;
		}
		else if (Cast<UBoxComponent>(HitComp) == RightGrateTrigger)
		{
			bIsOverlappingRightTrigger = false;
		}
	}
}

void AVent::ComponentBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	OverlappedPawns++;

	if (Cast<AShooter>(OtherActor))
	{
		if (Cast<UBoxComponent>(OverlappedComp) == LeftGrateTrigger)
		{
			bIsOverlappingLeftTrigger = true;
		}
		else if (Cast<UBoxComponent>(OverlappedComp) == RightGrateTrigger)
		{
			bIsOverlappingRightTrigger = true;
		}
	}
}