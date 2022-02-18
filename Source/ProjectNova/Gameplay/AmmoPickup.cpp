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
