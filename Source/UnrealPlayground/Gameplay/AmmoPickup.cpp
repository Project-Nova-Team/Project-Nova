// Fill out your copyright notice in the Description page of Project Settings.
#include "AmmoPickup.h"
#include "GameFramework/Pawn.h"
#include "../Weapon/CombatComponent.h"

// Sets default values
AAmmoPickup::AAmmoPickup()
{
	AmmoAmount = 10;
}

// Add ammo
void AAmmoPickup::InteractionEvent(const APawn* EventSender)
{
	/*UCombatComponent* PlayerCombatComponent = EventSender->FindComponentByClass<UCombatComponent>();

	if (PlayerCombatComponent != nullptr)
	{
		if (PlayerCombatComponent->GetPrimaryWeapon() != nullptr)
		{			
			if (AmmoAmount + PlayerCombatComponent->GetPrimaryWeapon()->GetExcessAmmo() <= PlayerCombatComponent->GetPrimaryWeapon()->GetMaxHeldAmmo())
			{
				SetInteractiveObjectHidden(true);

				// Right now we are only adding ammo to primary weapon. Let's ask design team what they want to do.
				PlayerCombatComponent->AddAmmmoToWeapon(PlayerCombatComponent->GetPrimaryWeapon(), AmmoAmount);
			} 
		}
	}*/
}

void AAmmoPickup::SetInteractiveObjectHidden(bool ActiveState)
{
	// Hides visible components
	SetActorHiddenInGame(ActiveState);

	// Disables collision components
	SetActorEnableCollision(false);

	// Stops the Actor from ticking
	SetActorTickEnabled(false);
}