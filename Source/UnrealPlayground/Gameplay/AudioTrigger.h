// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AudioTrigger.generated.h"

class UAudioComponent;
class USoundCue;

UCLASS()
class UNREALPLAYGROUND_API AAudioTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAudioTrigger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	/** Used to play audio. Set in bp*/
	UAudioComponent* AudioComponent;

	/** Stop Audio when player leaves trigger*/
	UPROPERTY(EditAnywhere)
	uint8 bStopAudioOnTriggerExit : 1;

	/** The actor that will activate this trigger.Set this to shooter. */
	UPROPERTY(EditAnywhere)
	AActor* TargetActor;

	UFUNCTION(BlueprintCallable)
	void SetComponents(UAudioComponent* AudioComponent);

	// Overlap
	UFUNCTION()
	void BeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void EndOverlap(AActor* OverlappedActor, AActor* OtherActor);
};
