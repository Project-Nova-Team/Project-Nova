 // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "MeleeComponent.h"
#include "Components/StaticMeshComponent.h"
#include "InteractiveObject.h"
#include "GameFramework/HUD.h"
#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>
#include "Components/BoxComponent.h"
#include "../ShooterHUD.h"
#include "GeneratorPiece.generated.h"

DECLARE_MULTICAST_DELEGATE(FGeneratorPieceHitEvent);

UCLASS()
class UNREALPLAYGROUND_API AGeneratorPiece : public AActor, public IInteractiveObject
{
	GENERATED_BODY()
	
public:	
	/** Set to true once player hits piece and it becomes fixed.*/
	uint8 bIsFixed : 1;

	// delegate
	FGeneratorPieceHitEvent OnGeneratorPieceHit;

	void InteractionEvent(APawn* EventSender) override;

	UStaticMeshComponent* GetMeshComponent() { return MeshComponent; };

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	UPROPERTY(EditAnywhere, Category = "Mesh")
	UStaticMesh* FixedMesh;

	UBoxComponent* Trigger;

	UStaticMeshComponent* MeshComponent;

	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void ShowHighlightedOutline();

	void SwapMeshToFixed();

};
