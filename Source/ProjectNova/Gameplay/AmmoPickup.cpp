#include "AmmoPickup.h"
#include "GameFramework/Pawn.h"
#include "../Player/Shooter.h"
#include "../Player/ShooterInventory.h"
#include "../Weapon/CombatComponent.h"
#include "Components/StaticMeshComponent.h"

AAmmoPickup::AAmmoPickup()
{
	Package.AmmoAmount = 10;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);

	ActionMappingName = "Interact";
}


void AAmmoPickup::InteractionEvent(APawn* EventSender)
{
	if (AShooter* Shooter = Cast<AShooter>(EventSender))
	{
		if (Package.GunClass.Get() != nullptr)
		{
			Shooter->GetInventory()->AddPack(Package);
		}

		Mesh->SetVisibility(false);
		Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AAmmoPickup::RecieveLookedAt(APawn* EventSender)
{
	if (CanInteract())
	{
		BindingIndex = EventSender->InputComponent->BindAction<FShooterBindingEvent>(ActionMappingName, 
			IE_Pressed, this, &AAmmoPickup::InteractionEvent, EventSender).GetHandle();
	}
}

void AAmmoPickup::RecieveLookedAway(APawn* EventSender, int32 MappingIndexToRemove)
{
	// Remove the delegate tied to the this object's desired ActionMapping
	EventSender->InputComponent->RemoveActionBindingForHandle(MappingIndexToRemove);
}
