// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "AudioTrigger.h"

// Sets default values
AAudioTrigger::AAudioTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAudioTrigger::BeginPlay()
{
	Super::BeginPlay();
	// bind delegates
	OnActorBeginOverlap.AddDynamic(this, &AAudioTrigger::BeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &AAudioTrigger::EndOverlap);
}

// Called every frame
void AAudioTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAudioTrigger::SetComponents(UAudioComponent* Component)
{
	AudioComponent = Component;
 }

void AAudioTrigger::BeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OtherActor == TargetActor)
		AudioComponent->Play();
}

void AAudioTrigger::EndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (bStopAudioOnTriggerExit)
	{
		if (OtherActor == TargetActor)
			AudioComponent->Stop();
	}
}

