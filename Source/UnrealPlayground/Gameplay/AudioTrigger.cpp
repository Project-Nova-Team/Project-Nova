#include "AudioTrigger.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

AAudioTrigger::AAudioTrigger()
{
	AudioComponent = CreateDefaultSubobject<UAudioComponent>("Audio");
	AudioComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

void AAudioTrigger::ExecuteTrigger(APawn* Sender)
{
	Super::ExecuteTrigger(Sender);
	AudioComponent->Play();
}