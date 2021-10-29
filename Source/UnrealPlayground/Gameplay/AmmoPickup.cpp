#include "AmmoPickup.h"
#include "GameFramework/Pawn.h"
#include "../Player/Shooter.h"

AAmmoPickup::AAmmoPickup()
{
	AmmoAmount = 10;
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
	}
}
