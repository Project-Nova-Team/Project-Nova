// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../ShooterGameMode.h"
#include "Components/BoxComponent.h"
#include "Door.h"
#include "Elevator.generated.h"

struct FDelayedActionHandle;
class AInteractiveButton;
class UAudioComponent;

UENUM()
enum EElevatorState
{
	EES_Idle,
	EES_Ascending,
	EES_Descending
};

UCLASS()
class UNREALPLAYGROUND_API AElevator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AElevator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ElevatorBody;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trigger", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* Trigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sound", meta = (AllowPrivateAccess = "true"))
	UAudioComponent* AudioComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ButtonPanel;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door", meta = (AllowPrivateAccess = "true"))
	ADoor* Door;

	UPROPERTY(BlueprintReadWrite)
	AInteractiveButton* UpButton;

	UPROPERTY(BlueprintReadWrite)
	AInteractiveButton* DownButton;

	/** How many pawns are currently overlapped with this actor*/
	int32 CurrentPawnCount;

	/** Delayed action handle for ease of LERP*/
	FDelayedActionHandle* Handle;

	uint8 bIsMoving;

	/** Whether or not the button is currently locked*/
	UPROPERTY(EditAnywhere, Category = "Elevator")
	uint8 bIsLocked : 1;

	UPROPERTY(EditAnywhere)
	float MoveDuration;

	UPROPERTY(EditAnywhere)
	float MaxHeight;

	/** Invoked by ActorBeginOverlap*/
	UFUNCTION()
	virtual void ActorStartOverlap(AActor* OverlappedActor, AActor* OtherActor);

	/** Invoked by ActorEndOverlap*/
	UFUNCTION()
	virtual void ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor);

	/** Move Elevator*/
	void OverTimeTransition(bool bMovesUp, FVector InitialOffset);

	UFUNCTION()
	void OnUpButtonPressed(APawn* EventSender);

	UFUNCTION()
	void OnDownButtonPressed(APawn* EventSender);
};
