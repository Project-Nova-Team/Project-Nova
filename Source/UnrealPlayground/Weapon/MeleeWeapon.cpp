// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeWeapon.h"
#include "Weapon.h"

// Sets default values
AMeleeWeapon::AMeleeWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMeleeWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMeleeWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMeleeWeapon::InteractionEvent(const APawn* EventSender)
{
	//If a pawn who sent the interaction has a combat component, pick the weapon up
	UShooterCombatComponent* CombatComponent = EventSender->FindComponentByClass<UShooterCombatComponent>();
	if (CombatComponent != nullptr)
	{
		CombatComponent->PickUpMeleeWeapon(this);
	}
}
