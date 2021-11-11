// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Removable.h"
#include "GeneratorPiece.generated.h"

DECLARE_MULTICAST_DELEGATE(FGeneratorPieceHitEvent);

class UHealthComponent;

UCLASS()
class UNREALPLAYGROUND_API AGeneratorPiece : public AActor, public IRemovable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGeneratorPiece();

	/** Set to true once player hits piece and it becomes fixed.*/
	uint8 bIsFixed : 1;

	// delegate
	FGeneratorPieceHitEvent OnGeneratorPieceHit;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	UFUNCTION()
	void BroadcastDisable();

	UPROPERTY(Category = Health, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UHealthComponent* Health;

};
