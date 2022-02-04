#include "VaultObject.h"
#include <ProjectNova/Player/Shooter.h>

AVaultObject::AVaultObject()
{
	PrimaryActorTick.bCanEverTick = false;

	ObjectBody = CreateDefaultSubobject<UStaticMeshComponent>("Object Body");
	SetRootComponent(ObjectBody);

	Trigger = CreateDefaultSubobject<UBoxComponent>("Left Door");
	Trigger->AttachToComponent(ObjectBody, FAttachmentTransformRules::KeepRelativeTransform);
}

void AVaultObject::RecieveLookedAt(APawn* EventSender)
{
	UE_LOG(LogTemp, Warning, TEXT("Scanning Vault"));

	if (CanInteract())
	{
		for (int i = 0; i < Settings->GetActionMappings().Num(); i++)
		{
			// Find Action Mapping named Interact
			if (Settings->GetActionMappings()[i].ActionName == "Interact")
			{
				UE_LOG(LogTemp, Warning, TEXT("Interact Key: %s"), *InteractKey.GetFName().ToString());
				FInputActionKeyMapping TargetMapping = Settings->GetActionMappings()[i];
				if (TargetMapping.Key != InteractKey)
				{
					UE_LOG(LogTemp, Warning, TEXT("Key Before: %s"), *TargetMapping.Key.GetFName().ToString());
					// Remove any key bindings on current interact action
					Settings->RemoveActionMapping(TargetMapping);
					// Add custom keybinding
					Settings->AddActionMapping(FInputActionKeyMapping(TEXT("Interact"), InteractKey));
					Settings->SaveKeyMappings();
					UE_LOG(LogTemp, Warning, TEXT("Key After: %s"), *Settings->GetActionMappings()[i].Key.GetFName().ToString());
				}
			}
		}
	}
}

void AVaultObject::BeginPlay()
{
	OnActorBeginOverlap.AddDynamic(this, &AVaultObject::ActorStartOverlap);
	OnActorEndOverlap.AddDynamic(this, &AVaultObject::ActorEndOverlap);

	InteractKey = EKeys::SpaceBar;
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
	UE_LOG(LogTemp, Warning, TEXT("Overlap Vault Object"));
	bIsPlayerInTrigger = true;
}

void AVaultObject::ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	UE_LOG(LogTemp, Warning, TEXT("End Overlap Vault Object"));
	bIsPlayerInTrigger = false;
}
