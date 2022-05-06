#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../ShooterGameMode.h"
#include "Components/BoxComponent.h"
#include "Elevator.generated.h"

class UAudioComponent;

UCLASS()
class PROJECTNOVA_API AElevator : public AActor
{
	GENERATED_BODY()
	
public:	
	
	AElevator();

	UFUNCTION(BlueprintCallable)
	void StartElevator();

	UFUNCTION(BlueprintCallable)
	void StopElevator();

	void Tick(float DeltaTime) override;

protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	UStaticMeshComponent* ElevatorBody;

	/** Part of elevator that actually moves*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	UStaticMeshComponent* ElevatorPlatform;

	/** Looping movement sound*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sound")
	UAudioComponent* AudioComponent;

	/** Units per second the elevator moves down*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door")
	float Speed;

	/** Handle for movement*/
	FTimerHandle MovementHandle;
};
