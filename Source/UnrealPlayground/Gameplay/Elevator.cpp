#include "Elevator.h"
#include "Components/AudioComponent.h"
#include "InteractiveButton.h"
#include "../Utility/DelayedActionManager.h"

// Sets default values
AElevator::AElevator()
{
	FloorMesh = CreateDefaultSubobject<UStaticMeshComponent>("Floor");
	SetRootComponent(FloorMesh);

	Trigger = CreateDefaultSubobject<UBoxComponent>("Trigger");
	Trigger->AttachToComponent(FloorMesh, FAttachmentTransformRules::KeepRelativeTransform);

	AudioComponent = CreateDefaultSubobject<UAudioComponent>("Audio Component");
	AudioComponent->AttachToComponent(FloorMesh, FAttachmentTransformRules::KeepRelativeTransform);

	// makes sure audio doesn't play on start
	AudioComponent->bAutoActivate = false;

	// Move presets
	MaxHeight = 500;
	MoveDuration = 5;

	// Set the collision preset so the elevator can lift the player
	FloorMesh->SetCollisionProfileName(TEXT("PhysicsActor"));
}

// Called when the game starts or when spawned
void AElevator::BeginPlay()
{
	Super::BeginPlay();

	InitialOffset = FloorMesh->GetRelativeLocation();

	OnActorBeginOverlap.AddDynamic(this, &AElevator::ActorStartOverlap);
	OnActorEndOverlap.AddDynamic(this, &AElevator::ActorEndOverlap);

	Button->ButtonPressEvent.AddDynamic(this, &AElevator::OnButtonPressed);
}

void AElevator::ActorStartOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	CurrentPawnCount++;
}

void AElevator::ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	CurrentPawnCount--;
}

void AElevator::OverTimeTransition()
{
	bIsMoving = true;

	const FVector Direction = FVector::UpVector;

	float Offset;

	Offset = FMath::Lerp(0.f, MaxHeight, Handle->CurrentActionProgress);
	FloorMesh->SetRelativeLocation(InitialOffset + Direction * Offset);

	/*We've finished transitioning. Decide if we should start
	closing or opening again because something might have occured during the transition*/
	if (Handle->CurrentActionProgress >= 1.f)
	{
		bIsMoving = false;
	}
}

void AElevator::OnButtonPressed()
{
	if (CurrentPawnCount > 0)
	{
		Handle = GetWorld()->GetAuthGameMode<AShooterGameMode>()->GetDelayedActionManager()->StartOverTimeAction(
			this, &AElevator::OverTimeTransition, MoveDuration);
	}

}

