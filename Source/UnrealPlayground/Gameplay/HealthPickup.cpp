// Fill out your copyright notice in the Description page of Project Settings.

#include "HealthPickup.h"
#include "GameFramework/Pawn.h"
#include "./HealthComponent.h"

// Sets default values
AHealthPickup::AHealthPickup()
{
	HealAmount = 25;
}


void AHealthPickup::InteractionEvent(APawn* EventSender)
{
	UHealthComponent* PawnHealthComponent = EventSender->FindComponentByClass<UHealthComponent>();

	if (PawnHealthComponent != nullptr && !PawnHealthComponent->bIsFullHealth)
	{
		PawnHealthComponent->Heal(HealAmount);
	}
}