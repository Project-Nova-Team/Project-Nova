// Fill out your copyright notice in the Description page of Project Settings.

#include "HealthPickup.h"
#include "GameFramework/Pawn.h"
#include "./HealthComponent.h"

// Sets default values
AHealthPickup::AHealthPickup()
{
	HealAmount = 25;
}


void AHealthPickup::InteractionEvent(const APawn* EventSender)
{
	UHealthComponent* PawnHealthComponent = EventSender->FindComponentByClass<UHealthComponent>();

	if (PawnHealthComponent != nullptr)
	{
		// if health is not full, heal by heal amount
		PawnHealthComponent->Heal(HealAmount);

		//do idestroyable here
		//SetInteractiveObjectHidden(true);
	}
}