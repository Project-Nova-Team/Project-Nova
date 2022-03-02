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

	ActionMappingName = "Vault";
}

void AVaultObject::RecieveLookedAt(APawn* EventSender)
{
	if (CanInteract())
	{
		BindingIndex = EventSender->InputComponent->BindAction<FShooterBindingEvent>(ActionMappingName, 
			IE_Pressed, this, &AVaultObject::InteractionEvent, EventSender).GetHandle();
	}

	//if (CanInteract())
	//{
	//	for (int i = 0; i < Settings->GetActionMappings().Num(); i++)
	//	{
	//		// Find Action Mapping named Interact
	//		if (Settings->GetActionMappings()[i].ActionName == "Interact")
	//		{
	//			UE_LOG(LogTemp, Warning, TEXT("Interact Key: %s"), *InteractKey.GetFName().ToString());
	//			FInputActionKeyMapping TargetMapping = Settings->GetActionMappings()[i];
	//			if (TargetMapping.Key != InteractKey)
	//			{
	//				UE_LOG(LogTemp, Warning, TEXT("Key Before: %s"), *TargetMapping.Key.GetFName().ToString());
	//				// Remove any key bindings on current interact action
	//				Settings->RemoveActionMapping(TargetMapping);
	//				// Add custom keybinding
	//				Settings->AddActionMapping(FInputActionKeyMapping(TEXT("Interact"), InteractKey));
	//				Settings->SaveKeyMappings();
	//				UE_LOG(LogTemp, Warning, TEXT("Key After: %s"), *Settings->GetActionMappings()[i].Key.GetFName().ToString());
	//			}
	//		}
	//	}
	//}
}

void AVaultObject::RecieveLookedAway(APawn* EventSender, int32 MappingIndexToRemove)
{
	// Remove the delegate tied to the this object's desired ActionMapping
	EventSender->InputComponent->RemoveActionBindingForHandle(MappingIndexToRemove);
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
