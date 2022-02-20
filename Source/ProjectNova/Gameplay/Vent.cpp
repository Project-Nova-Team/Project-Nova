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

	ActionMappingName = "Interact";
}

void AVent::InteractionEvent(APawn* EventSender)
{
	UE_LOG(LogTemp, Warning, TEXT("Vent Interact"));

	AShooter* Shooter = Cast<AShooter>(EventSender);

	Shooter->GetStateMachine()->SetState("Venting");
}

void AVent::RecieveLookedAt(APawn* EventSender)
{
	if (CanInteract())
	{
		BindingIndex = EventSender->InputComponent->BindAction<FShooterBindingEvent>(ActionMappingName,
			IE_Pressed, this, &AVent::InteractionEvent, EventSender).GetHandle();
	}
}

void AVent::RecieveLookedAway(APawn* EventSender, int32 MappingIndexToRemove)
{
	// Remove the delegate tied to the this object's desired ActionMapping
	EventSender->InputComponent->RemoveActionBindingForHandle(MappingIndexToRemove);
}

void AVent::BeginPlay()
{

	bCanInteract = true; // temp

	Super::BeginPlay();
	Health->OnDeath.AddDynamic(this, &AVent::DisableGrateForDuration);
	OnActorBeginOverlap.AddDynamic(this, &AVent::ActorBeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &AVent::ActorEndOverlap);
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
	//bCanInteract = false; // cannot interact while grate is up
	Health->Revive();
	LeftGrate->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	RightGrate->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	LeftGrate->SetVisibility(true);
	RightGrate->SetVisibility(true);
	bIsDisabled = false;
	OnVentEnabled.Broadcast();
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