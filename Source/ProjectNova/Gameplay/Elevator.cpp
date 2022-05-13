#include "Elevator.h"
#include "Components/AudioComponent.h"
#include "Door.h"
#include "ScriptableInteractiveObject.h"

AElevator::AElevator()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	ElevatorBody = CreateDefaultSubobject<UStaticMeshComponent>("Elevator Body");
	SetRootComponent(ElevatorBody);

	ElevatorPlatform = CreateDefaultSubobject<UStaticMeshComponent>("Platform");
	ElevatorPlatform->SetupAttachment(ElevatorBody);
	ElevatorPlatform->SetCollisionProfileName(TEXT("BlockAllDynamic"));

	AudioComponent = CreateDefaultSubobject<UAudioComponent>("Audio Component");
	AudioComponent->SetupAttachment(ElevatorPlatform);
	AudioComponent->bAutoActivate = false;

	Speed = 300.f;
}

void AElevator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const float Direction = bMoveUp ? -1 : 1;
	const FVector Current = ElevatorPlatform->GetRelativeLocation();
	const float Delta = GetWorld()->GetDeltaSeconds() * Speed * Direction;

	ElevatorPlatform->SetRelativeLocation(FVector(Current.X, Current.Y, Current.Z - Delta));
}

void AElevator::StartElevator()
{
	AudioComponent->Play();
	SetActorTickEnabled(true);
}

void AElevator::StopElevator()
{
	AudioComponent->Stop();
	SetActorTickEnabled(false);
}