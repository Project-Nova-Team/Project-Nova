// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Runtime/Engine/Classes/Components/BoxComponent.h>
#include "Kismet/KismetMathLibrary.h"
#include "Door.generated.h"

class UStaticMeshComponent;

UCLASS()
class UNREALPLAYGROUND_API ADoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** The duration at which the door opens/Closes. ~1.0 for slow-ish, .5 for very fast.*/
	UPROPERTY(EditAnywhere)
		float DoorMoveDuration;

private:
	UFUNCTION(BlueprintCallable)
	void SetComponents(UStaticMeshComponent* LeftDoor, UStaticMeshComponent* RightDoor, UBoxComponent* BoxTrigger);

	void OpenDoor(float DeltaTime);

	FVector LeftDoorStartingLocation;

	FVector RightDoorStartingLocation;

	FVector LeftDoorEndLocation;

	FVector RightDoorEndLocation;

	// Overlap
	UFUNCTION()
	void EnterOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult);

	UFUNCTION()
	void ExitOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex);

	UPROPERTY()
	UStaticMeshComponent* LeftDoorObject;
	UPROPERTY()
	UStaticMeshComponent* RightDoorObject;
	UPROPERTY()
	UBoxComponent* Trigger;

	// used as timer for lerp
	float TimeTracker;

	//If true, the door is opening, if false, it is closing
	uint8 bOpen : 1;

	uint8 Finished : 1;

	FVector LeftDoorCurrentPosition;

	FVector RightDoorCurrentPosition;
};
