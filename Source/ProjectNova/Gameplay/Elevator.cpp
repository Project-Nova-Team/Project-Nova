#include "Elevator.h"
#include "Components/AudioComponent.h"
#include "Door.h"
#include "ScriptableInteractiveObject.h"

AElevator::AElevator()
{
	ElevatorBody = CreateDefaultSubobject<UStaticMeshComponent>("Elevator Body");
	SetRootComponent(ElevatorBody);

	ElevatorPlatform = CreateDefaultSubobject<UStaticMeshComponent>("Platform");
	ElevatorPlatform->SetupAttachment(ElevatorBody);
	ElevatorPlatform->SetCollisionProfileName(TEXT("BlockAllDynamic"));

	AudioComponent = CreateDefaultSubobject<UAudioComponent>("Audio Component");
	AudioComponent->SetupAttachment(ElevatorBody);
	AudioComponent->bAutoActivate = false;

	Speed = 300.f;
}

void AElevator::StartElevator()
{
	AudioComponent->Play();
	MovementHandle = GetWorldTimerManager().SetTimerForNextTick(this, &AElevator::MoveDown);
}

void AElevator::StopElevator()
{
	AudioComponent->Stop();
	GetWorldTimerManager().ClearTimer(MovementHandle);
}

void AElevator::MoveDown()
{
	const FVector Current = ElevatorPlatform->GetRelativeLocation();
	const float Delta = GetWorld()->GetDeltaSeconds() * Speed;

	ElevatorPlatform->SetRelativeLocation(FVector(Current.X, Current.Y, Current.Z - Delta));

	MovementHandle = GetWorldTimerManager().SetTimerForNextTick(this, &AElevator::MoveDown);
}