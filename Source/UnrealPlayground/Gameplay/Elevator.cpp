#include "Elevator.h"
#include "Components/AudioComponent.h"
#include "InteractiveButton.h"
#include "../Utility/DelayedActionManager.h"

// Sets default values
AElevator::AElevator()
{
	ElevatorBody = CreateDefaultSubobject<UStaticMeshComponent>("Elevator Body");
	SetRootComponent(ElevatorBody);

	Trigger = CreateDefaultSubobject<UBoxComponent>("Trigger");
	Trigger->AttachToComponent(ElevatorBody, FAttachmentTransformRules::KeepRelativeTransform);

	AudioComponent = CreateDefaultSubobject<UAudioComponent>("Audio Component");
	AudioComponent->AttachToComponent(ElevatorBody, FAttachmentTransformRules::KeepRelativeTransform);

	Door = CreateDefaultSubobject<ADoor>("Door");
	Door->AttachToComponent(ElevatorBody, FAttachmentTransformRules::KeepRelativeTransform);

	ButtonPanel = CreateDefaultSubobject<UStaticMeshComponent>("Button Panel");
	ButtonPanel->AttachToComponent(ElevatorBody, FAttachmentTransformRules::KeepRelativeTransform);

	UpButton = CreateDefaultSubobject<AInteractiveButton>("Up Button");
	UpButton->AttachToComponent(ButtonPanel, FAttachmentTransformRules::KeepRelativeTransform);

	DownButton = CreateDefaultSubobject<AInteractiveButton>("Down Button");
	DownButton->AttachToComponent(ButtonPanel, FAttachmentTransformRules::KeepRelativeTransform);

	// makes sure audio doesn't play on start
	AudioComponent->bAutoActivate = false;

	// Move presets
	MaxHeight = 500;
	MoveDuration = 5;

	// Set the collision preset so the elevator can lift the player
	ElevatorBody->SetCollisionProfileName(TEXT("PhysicsActor"));
}

// Called when the game starts or when spawned
void AElevator::BeginPlay()
{
	Super::BeginPlay();

	InitialOffset = ElevatorBody->GetRelativeLocation();

	OnActorBeginOverlap.AddDynamic(this, &AElevator::ActorStartOverlap);
	OnActorEndOverlap.AddDynamic(this, &AElevator::ActorEndOverlap);

	UpButton->OnInteract.AddUObject(this, &AElevator::OnUpButtonPressed);
	DownButton->OnInteract.AddUObject(this, &AElevator::OnDownButtonPressed);
}

void AElevator::ActorStartOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	CurrentPawnCount++;
}

void AElevator::ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	CurrentPawnCount--;
}

void AElevator::OverTimeTransition(bool bMovesUp)
{
	bIsMoving = true;
	FVector Direction;
	float Offset;

	if (bMovesUp)
		Direction = FVector::UpVector;
	else
		Direction = FVector::DownVector;

	Offset = FMath::Lerp(0.f, MaxHeight, Handle->CurrentActionProgress);
	ElevatorBody->SetRelativeLocation(InitialOffset + Direction * Offset);

	/*We've finished transitioning. Decide if we should start
	closing or opening again because something might have occured during the transition*/
	if (Handle->CurrentActionProgress >= 1.f)
	{
		bIsMoving = false;
	}
}

void AElevator::OnUpButtonPressed(APawn* EventSender)
{
	if (CurrentPawnCount > 0)
	{
		Handle = GetWorld()->GetAuthGameMode<AShooterGameMode>()->GetDelayedActionManager()->StartOverTimeAction(
			this, &AElevator::OverTimeTransition, MoveDuration, true);
	}
}

void AElevator::OnDownButtonPressed(APawn* EventSender)
{
	if (CurrentPawnCount > 0)
	{
		Handle = GetWorld()->GetAuthGameMode<AShooterGameMode>()->GetDelayedActionManager()->StartOverTimeAction(
			this, &AElevator::OverTimeTransition, MoveDuration, false);
	}
}

