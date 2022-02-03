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
	/*DECLARE_DELEGATE_OneParam(FCustomInputDelegate, APawn * EventSender);
	EventSender->InputComponent->BindAction<FCustomInputDelegate>("Vault", IE_Pressed, this, &AVaultObject::InteractionEvent, EventSender);*/
	
	if (bIsPlayerInTrigger)
	{
		if (!Settings) { return; }

		for (FInputActionKeyMapping m : Settings->GetActionMappings())
		{
			if (m.ActionName == "Interact")
			{
				m.Key = EKeys::L;
				InteractKey = m.Key;
				UE_LOG(LogTemp, Warning, TEXT("%s"), *m.Key.GetFName().ToString());
			}
		}

		// maybe try getting all keys of an action mapping?
		// get all keys of an action mappying by name and remove them using remove action mapping
		// then add preferred key?

		Settings->RemoveActionMapping(DefaultInteractMapping);
		Settings->AddActionMapping(FInputActionKeyMapping(TEXT("Interact"), InteractKey));
		Settings->SaveKeyMappings();
	}
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

		/*UE_LOG(LogTemp, Warning, TEXT("0 %s"), *EventSender->InputComponent->GetActionBinding(0).GetActionName().ToString());
		UE_LOG(LogTemp, Warning, TEXT("1 %s"), *EventSender->InputComponent->GetActionBinding(1).GetActionName().ToString());
		UE_LOG(LogTemp, Warning, TEXT("2 %s"), *EventSender->InputComponent->GetActionBinding(2).GetActionName().ToString());
		UE_LOG(LogTemp, Warning, TEXT("3 %s"), *EventSender->InputComponent->GetActionBinding(3).GetActionName().ToString());
		UE_LOG(LogTemp, Warning, TEXT("4 %s"), *EventSender->InputComponent->GetActionBinding(4).GetActionName().ToString());
		UE_LOG(LogTemp, Warning, TEXT("5 %s"), *EventSender->InputComponent->GetActionBinding(5).GetActionName().ToString());
		UE_LOG(LogTemp, Warning, TEXT("6 %s"), *EventSender->InputComponent->GetActionBinding(6).GetActionName().ToString());
		UE_LOG(LogTemp, Warning, TEXT("7 %s"), *EventSender->InputComponent->GetActionBinding(7).GetActionName().ToString());
		UE_LOG(LogTemp, Warning, TEXT("8 %s"), *EventSender->InputComponent->GetActionBinding(8).GetActionName().ToString());
		UE_LOG(LogTemp, Warning, TEXT("9 %s"), *EventSender->InputComponent->GetActionBinding(9).GetActionName().ToString());
		UE_LOG(LogTemp, Warning, TEXT("10 %s"), *EventSender->InputComponent->GetActionBinding(10).GetActionName().ToString());
		UE_LOG(LogTemp, Warning, TEXT("11 %s"), *EventSender->InputComponent->GetActionBinding(11).GetActionName().ToString());
		UE_LOG(LogTemp, Warning, TEXT("12 %s"), *EventSender->InputComponent->GetActionBinding(12).GetActionName().ToString());
		UE_LOG(LogTemp, Warning, TEXT("13 %s"), *EventSender->InputComponent->GetActionBinding(13).GetActionName().ToString());
		UE_LOG(LogTemp, Warning, TEXT("14 %s"), *EventSender->InputComponent->GetActionBinding(14).GetActionName().ToString());
		UE_LOG(LogTemp, Warning, TEXT("15 %s"), *EventSender->InputComponent->GetActionBinding(15).GetActionName().ToString());
		UE_LOG(LogTemp, Warning, TEXT("16 %s"), *EventSender->InputComponent->GetActionBinding(16).GetActionName().ToString());
		UE_LOG(LogTemp, Warning, TEXT("17 %s"), *EventSender->InputComponent->GetActionBinding(17).GetActionName().ToString());
		UE_LOG(LogTemp, Warning, TEXT("18 %s"), *EventSender->InputComponent->GetActionBinding(18).GetActionName().ToString());*/
		/*UE_LOG(LogTemp, Warning, TEXT("19 %s"), *EventSender->InputComponent->GetActionBinding(19).GetActionName().ToString());
		UE_LOG(LogTemp, Warning, TEXT("20 %s"), *EventSender->InputComponent->GetActionBinding(20).GetActionName().ToString());
		UE_LOG(LogTemp, Warning, TEXT("21 %s"), *EventSender->InputComponent->GetActionBinding(21).GetActionName().ToString());
		UE_LOG(LogTemp, Warning, TEXT("22 %s"), *EventSender->InputComponent->GetActionBinding(22).GetActionName().ToString());
		UE_LOG(LogTemp, Warning, TEXT("23 %s"), *EventSender->InputComponent->GetActionBinding(23).GetActionName().ToString());
		UE_LOG(LogTemp, Warning, TEXT("24 %s"), *EventSender->InputComponent->GetActionBinding(24).GetActionName().ToString());
		UE_LOG(LogTemp, Warning, TEXT("25 %s"), *EventSender->InputComponent->GetActionBinding(25).GetActionName().ToString());
		UE_LOG(LogTemp, Warning, TEXT("26 %s"), *EventSender->InputComponent->GetActionBinding(26).GetActionName().ToString());*/
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
