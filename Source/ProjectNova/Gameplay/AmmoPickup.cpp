#include "AmmoPickup.h"
#include "GameFramework/Pawn.h"
#include "../Player/Shooter.h"
#include "Components/StaticMeshComponent.h"

AAmmoPickup::AAmmoPickup()
{
	AmmoAmount = 10;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);
}


void AAmmoPickup::InteractionEvent(APawn* EventSender)
{
	AShooter* Shooter = Cast<AShooter>(EventSender);
	
	if (Shooter != nullptr)
	{
		switch (GunType)
		{
		case WC_Pistol:
			Shooter->GetInventory()->PistolAmmo += AmmoAmount;
			break;
		case WC_Shotgun:
			Shooter->GetInventory()->ShotgunAmmo += AmmoAmount;
			break;
		case WC_Rifle:
			Shooter->GetInventory()->RifleAmmo += AmmoAmount;
		}

		if (Shooter->HasGunOfType(GunType))
		{
			Shooter->LoadAmmoOnPickup(GunType);
		}

		RemoveSelf(this);
	}
}

void AAmmoPickup::RecieveLookedAt(APawn* EventSender)
{
	UE_LOG(LogTemp, Warning, TEXT("Scanning Ammo"));

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
