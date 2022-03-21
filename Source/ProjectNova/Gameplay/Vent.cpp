#include "Vent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "HealthComponent.h"
#include "InteractableFusebox.h"
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

	Fusebox = CreateDefaultSubobject<UChildActorComponent>("Fusebox");
	Fusebox->AttachToComponent(LeftFrame, FAttachmentTransformRules::KeepRelativeTransform);
	Fusebox->SetChildActorClass(AInteractableFusebox::StaticClass());

	DisableDuration = 10.f;

	CrawlSpeed = 125.f;
}

void AVent::InteractionEvent(APawn* EventSender)
{
	if (AShooter* Shooter = Cast<AShooter>(EventSender))
	{
		Shooter->GetStateMachine()->SetState("Venting");
		bCanInteract = false;
		LeftGrate->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		RightGrate->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		OnInteract.Broadcast(EventSender);
	}
}

void AVent::ReceiveFuseboxFixed(APawn* EventSender)
{
	if (AShooter* Shooter = Cast<AShooter>(EventSender))
	{
		DisableGrateForDuration();
	}
}

void AVent::BeginPlay()
{
	Super::BeginPlay();
	Health->OnDeath.AddDynamic(this, &AVent::DisableGrateForDuration);
	LeftGrateTrigger->OnComponentBeginOverlap.AddDynamic(this, &AVent::ComponentBeginOverlap);
	LeftGrateTrigger->OnComponentEndOverlap.AddDynamic(this, &AVent::ComponentEndOverlap);
	RightGrateTrigger->OnComponentBeginOverlap.AddDynamic(this, &AVent::ComponentBeginOverlap);
	RightGrateTrigger->OnComponentEndOverlap.AddDynamic(this, &AVent::ComponentEndOverlap);

	FuseboxRef = Cast<AInteractableFusebox>(Fusebox->GetChildActor());
	FuseboxRef->OnInteract.AddUObject(this, &AVent::ReceiveFuseboxFixed);
}

void AVent::DisableGrateForDuration()
{
	bCanInteract = true; // can interact while grate is down
	LeftGrate->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	RightGrate->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	LeftGrate->SetVisibility(false);
	RightGrate->SetVisibility(false);
	OnVentDisabled.Broadcast();
	bIsDisabled = true;
	bDelayRunning = true;
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
	FuseboxRef->SetCanInteract(true);
	bCanInteract = false;
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

			if(bCanInteract) // we have left the trigger without venting
				GetWorld()->GetAuthGameMode<AShooterGameMode>()->GetDelayedActionManager()->StartDelayedAction(this, &AVent::MaybeReEnableGrate, DisableDuration);
		}
		else if (Cast<UBoxComponent>(HitComp) == RightGrateTrigger)
		{
			bIsOverlappingRightTrigger = false;

			if(bCanInteract) // we have left the trigger without venting
				GetWorld()->GetAuthGameMode<AShooterGameMode>()->GetDelayedActionManager()->StartDelayedAction(this, &AVent::MaybeReEnableGrate, DisableDuration);
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
			UE_LOG(LogTemp, Warning, TEXT("LeftGrate"));
		}
		else if (Cast<UBoxComponent>(OverlappedComp) == RightGrateTrigger)
		{
			bIsOverlappingRightTrigger = true;
			UE_LOG(LogTemp, Warning, TEXT("RightGrate"));
		}
	}
}