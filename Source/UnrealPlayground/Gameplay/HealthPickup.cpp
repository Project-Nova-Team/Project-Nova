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
}


void AHealthPickup::InteractionEvent(APawn* EventSender)
{
	UHealthComponent* PawnHealthComponent = EventSender->FindComponentByClass<UHealthComponent>();

	if (PawnHealthComponent != nullptr && !PawnHealthComponent->bIsFullHealth)
	{
		PawnHealthComponent->Heal(HealAmount);
		Mesh->SetVisibility(false);
		Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}