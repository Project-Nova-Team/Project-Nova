// Fill out your copyright notice in the Description page of Project Settings.

#include "HealthPickup.h"
#include "GameFramework/Pawn.h"
#include "./HealthComponent.h"

// Sets default values
AHealthPickup::AHealthPickup()
{
	HealAmount = 25;
}

//Heal
void AHealthPickup::InteractionEvent(const APawn* EventSender) 
{		
	UHealthComponent* PawnHealthComponent = EventSender->FindComponentByClass<UHealthComponent>();

	if (PawnHealthComponent != nullptr)
	{
		// CHECK IF HEALTH IS FULL BEFORE YOU CAN PICK UP!

		PawnHealthComponent->Heal(HealAmount);

		// Debug message
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, "Healed");

		// Turn this into method somewhere. We could make a destroyable class and inherit from it or something
		const FVector DestroyPos = FVector(0, 0, -10000);

		SetActorLocation(DestroyPos);
	}	
}

