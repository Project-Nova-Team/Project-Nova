#include "HealthPickup.h"
#include "GameFramework/Pawn.h"
#include "./HealthComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AHealthPickup::AHealthPickup()
{
	HealAmount = 25;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);

	ActionMappingName = "Interact";
}




void AHealthPickup::InteractionEvent(APawn* EventSender)
{
	if (CanInteract())
	{
		UHealthComponent* PawnHealthComponent = EventSender->FindComponentByClass<UHealthComponent>();

		if (PawnHealthComponent != nullptr && !PawnHealthComponent->bIsFullHealth)
		{
			PawnHealthComponent->Heal(HealAmount);
			RemoveSelf(this);
		}
	}
}

void AHealthPickup::RecieveLookedAt(APawn* EventSender)
{
	if (CanInteract())
	{
		BindingIndex = EventSender->InputComponent->BindAction<FShooterBindingEvent>(ActionMappingName,
			IE_Pressed, this, &AHealthPickup::InteractionEvent, EventSender).GetHandle();
	}
}

void AHealthPickup::RecieveLookedAway(APawn* EventSender, int32 MappingIndexToRemove)
{
	EventSender->InputComponent->RemoveActionBindingForHandle(MappingIndexToRemove);
}
