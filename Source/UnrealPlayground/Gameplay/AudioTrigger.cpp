#include "AudioTrigger.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// Sets default values
AAudioTrigger::AAudioTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AAudioTrigger::BeginPlay()
{
	Super::BeginPlay();
	// bind delegates
	OnActorBeginOverlap.AddDynamic(this, &AAudioTrigger::BeginOverlap);
}

void AAudioTrigger::SetComponents(UAudioComponent* Component)
{
	AudioComponent = Component;
 }

void AAudioTrigger::BeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OtherActor == TargetActor)
		UGameplayStatics::PlaySoundAtLocation(this, AudioComponent->Sound, AudioComponent->GetComponentLocation());
}

