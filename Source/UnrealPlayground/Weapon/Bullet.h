// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UObject/ConstructorHelpers.h"
#include "Bullet.generated.h"

class UStaticMeshComponent;

UCLASS()
class UNREALPLAYGROUND_API ABullet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABullet();
	virtual void Tick(float DeltaTime) override;

	// If it would be possible to pass this in through constructor, that would be better
	FHitResult RaycastHit;

private:
	UStaticMeshComponent* Mesh;
};
