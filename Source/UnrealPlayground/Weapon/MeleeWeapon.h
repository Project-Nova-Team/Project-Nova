// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.h"
#include "MeleeWeapon.generated.h"

class AWeapon;

UCLASS()
class UNREALPLAYGROUND_API AMeleeWeapon : public AWeapon
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMeleeWeapon();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void InteractionEvent(const APawn* EventSender) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

};
