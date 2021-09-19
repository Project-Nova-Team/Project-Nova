// Fill out your copyright notice in the Description page of Project Settings.
#include "AmmoPickup.h"
#include "GameFramework/Pawn.h"
#include "../Weapon/ShooterCombatComponent.h"

// Sets default values
AAmmoPickup::AAmmoPickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AmmoAmount = 10;

}

// Called when the game starts or when spawned
void AAmmoPickup::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAmmoPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Add ammo
void AAmmoPickup::InteractionEvent(const APawn* EventSender)
{
	UShooterCombatComponent* PlayerCombatComponent = EventSender->FindComponentByClass<UShooterCombatComponent>();

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
	}
}
	

