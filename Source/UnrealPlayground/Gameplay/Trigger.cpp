#include "Trigger.h"
#include "Components/BoxComponent.h"

ATrigger::ATrigger()
{
	TriggerVolume = CreateDefaultSubobject<UBoxComponent>("Trigger");
	TriggerVolume->SetCollisionProfileName("OverlapOnlyPawn");
	SetRootComponent(TriggerVolume);

	bStartActive = true;
	bIsActive = true;
}

void ATrigger::BeginPlay()
{
	Super::BeginPlay();

	if (TriggerWhiteList.Num() == 0)
	{
		FString Log;
		GetName(Log);
		Log += TEXT(" does not have any actors attached to the class white list so it will never trigger! Assign pawn subclasses in blueprint");
		UE_LOG(LogTemp, Error, TEXT("%s"), *Log);

		bStartActive = false;
	}

	SetActive(bStartActive);
	
	OnActorBeginOverlap.AddDynamic(this, &ATrigger::BeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &ATrigger::EndOverlap);
}

void ATrigger::BeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (bIsActive && TriggerWhiteList.Contains(OtherActor->GetClass()))
	{
		//Forces other actor to be a pawn type. This will cause crashes if other actor is not a pawn
		ExecuteTrigger(Cast<APawn>(OtherActor));
	}
}

void ATrigger::EndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (TriggerWhiteList.Contains(OtherActor->GetClass()))
	{
		OnTriggerExited.Broadcast();
	}
}

void ATrigger::ExecuteTrigger(APawn* Sender)
{
	//Invokes blueprint logic event
	OnTriggerActivated.Broadcast();

	//Child classes fill out logic here
	if (!bTriggerMoreThanOnce)
	{
		SetActive(false);
	}
}

void ATrigger::SetActive(const bool Value)
{
	if (bIsActive != Value)
	{
		bIsActive = Value;

		if (bIsActive)
		{
			TriggerVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}

		else
		{
			TriggerVolume->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		}
	}
}