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

	BoxTrigger = Cast<UBoxComponent>(GetComponentByClass(UBoxComponent::StaticClass()));

	AudioComponent = Cast<UAudioComponent>(GetComponentByClass(UAudioComponent::StaticClass()));
}

void AAudioTrigger::RemoveSelf(AActor* Actor)
{
	BoxTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AAudioTrigger::RestoreSelf(AActor* Actor)
{
	BoxTrigger->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AAudioTrigger::SetComponents(UAudioComponent* Component)
{
	AudioComponent = Component;
 }

void AAudioTrigger::BeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OtherActor == TargetActor)
		UGameplayStatics::PlaySoundAtLocation(this, AudioComponent->Sound, AudioComponent->GetComponentLocation());

	if (bDisappears)
		RemoveSelf(this);
}

