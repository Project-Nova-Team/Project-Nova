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
			// CHECK IF AMMO IS FULL BEFORE YOU CAN PICK UP!
			
			// Debug message
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Purple, "Ammo Pickup");

			// Right now we are only adding ammo to primary weapon. Let's ask design team what they want to do.
			PlayerCombatComponent->AddAmmmoToWeapon(PlayerCombatComponent->GetPrimaryWeapon(), AmmoAmount);

			const FVector DestroyPos = FVector(0, 0, -10000);

			SetActorLocation(DestroyPos);
		}
	}*/
}
	

