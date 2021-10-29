// Fill out your copyright notice in the Description page of Project Settings.
#include "Door.h"
#include "Components/StaticMeshComponent.h"


// Sets default values
ADoor::ADoor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ADoor::BeginPlay()
{
	Super::BeginPlay();
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &ADoor::EnterOverlap);
	Trigger->OnComponentEndOverlap.AddDynamic(this, &ADoor::ExitOverlap);

	DoorMoveDuration = .7f;
}

// Called every frame
void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	OpenDoor(DeltaTime);
}

void ADoor::SetComponents(UStaticMeshComponent* LeftDoor, UStaticMeshComponent* RightDoor, UBoxComponent* BoxTrigger)
{
	LeftDoorObject = LeftDoor;
	RightDoorObject = RightDoor;
	Trigger = BoxTrigger;

	LeftDoorStartingLocation = LeftDoorObject->GetRelativeLocation();
	RightDoorStartingLocation = RightDoorObject->GetRelativeLocation();

	LeftDoorEndLocation = LeftDoorStartingLocation - FVector(50, LeftDoorStartingLocation.Y, LeftDoorStartingLocation.Z);
	RightDoorEndLocation = RightDoorStartingLocation + FVector(50, RightDoorStartingLocation.Y, RightDoorStartingLocation.Z);

	LeftDoorCurrentPosition = LeftDoorObject->GetRelativeLocation();
	RightDoorCurrentPosition = RightDoorObject->GetRelativeLocation();
}

void ADoor::OpenDoor(float DeltaTime)
{
	const float Alpha = FMath::Clamp(TimeTracker / DoorMoveDuration, 0.f, 1.f);
	TimeTracker += DeltaTime;
	// player is in trigger
	if (bOpen)
	{
		// if door has not finished opening
		if (!Finished)
		{
			// if door open duration is less than the timer that started at the beginning
			// of this method,
			if (TimeTracker < DoorMoveDuration)
			{
				// Lerp from the position saved when player entered trigger to end pos
				LeftDoorObject->SetRelativeLocation(FMath::Lerp(LeftDoorCurrentPosition, LeftDoorEndLocation, Alpha));
				RightDoorObject->SetRelativeLocation(FMath::Lerp(RightDoorCurrentPosition, RightDoorEndLocation, Alpha));
			}
			//snap into place when door gets close to end (achillies and the tortoise paradox)
			else
			{
				LeftDoorObject->SetRelativeLocation(LeftDoorEndLocation);
				RightDoorObject->SetRelativeLocation(RightDoorEndLocation);
			}
		}
	}
	// player is out of trigger
	else
	{
		// If door wasn't finished opening/closing
		if (!Finished)
		{
			// if door open duration is less than the timer that started at the beginning
			// of this method,
			if (TimeTracker < DoorMoveDuration)
			{
				// move doors from current pos to start pos
				LeftDoorObject->SetRelativeLocation(FMath::Lerp(LeftDoorCurrentPosition, LeftDoorStartingLocation, Alpha));
				RightDoorObject->SetRelativeLocation(FMath::Lerp(RightDoorCurrentPosition, RightDoorStartingLocation, Alpha));
			}
			//snap into place when door gets close to end (achillies and the tortoise paradox)
			else
			{
				LeftDoorObject->SetRelativeLocation(LeftDoorStartingLocation);
				RightDoorObject->SetRelativeLocation(RightDoorStartingLocation);
			}
		}

		// if doors have reached their endpoint finished is true.
		if (Alpha == 1.f)
		{
			Finished = true;
		}
	}
}


void ADoor::EnterOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	bOpen = true;
	Finished = false;
	LeftDoorCurrentPosition = LeftDoorObject->GetRelativeLocation();
	RightDoorCurrentPosition = RightDoorObject->GetRelativeLocation();
	TimeTracker = 0;
}

void ADoor::ExitOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	bOpen = false;
	Finished = false;
	LeftDoorCurrentPosition = LeftDoorObject->GetRelativeLocation();
	RightDoorCurrentPosition = RightDoorObject->GetRelativeLocation();
	TimeTracker = 0;
}

