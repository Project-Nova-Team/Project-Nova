#include "VaultObject.h"
#include <ProjectNova/Player/Shooter.h>

AVaultObject::AVaultObject()
{
	PrimaryActorTick.bCanEverTick = false;

	ObjectBody = CreateDefaultSubobject<UStaticMeshComponent>("Object Body");
	SetRootComponent(ObjectBody);

	Trigger = CreateDefaultSubobject<UBoxComponent>("Trigger");
	Trigger->AttachToComponent(ObjectBody, FAttachmentTransformRules::KeepRelativeTransform);

	Offset = CreateDefaultSubobject<USceneComponent>(TEXT("Offset"));
	Offset->AttachToComponent(ObjectBody, FAttachmentTransformRules::KeepRelativeTransform);
}

void AVaultObject::BeginPlay()
{
	OnActorBeginOverlap.AddDynamic(this, &AVaultObject::ActorStartOverlap);
	OnActorEndOverlap.AddDynamic(this, &AVaultObject::ActorEndOverlap);
}

void AVaultObject::InteractionEvent(APawn* EventSender)
{
	if (CanInteract())
	{
		UE_LOG(LogTemp, Warning, TEXT("Vault Interact"));
	}
}

void AVaultObject::ActorStartOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	bIsPlayerInTrigger = true;
}

void AVaultObject::ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	bIsPlayerInTrigger = false;
}
