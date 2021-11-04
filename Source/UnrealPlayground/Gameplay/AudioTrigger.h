// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Removable.h"
#include <Runtime/Engine/Classes/Components/BoxComponent.h>
#include "AudioTrigger.generated.h"

class UAudioComponent;
class USoundCue;
class UGameplayStatistics;

UCLASS()
class UNREALPLAYGROUND_API AAudioTrigger : public AActor, public IRemovable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAudioTrigger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	UBoxComponent* BoxTrigger;

	virtual void RemoveSelf(AActor* Actor) override;

	virtual void RestoreSelf(AActor* Actor) override;

	/** Used to play audio. Set in bp*/
	UAudioComponent* AudioComponent;

	/** Set true if this object should be disabled after player leaves the trigger.*/
	UPROPERTY(EditAnywhere)
	uint8 bDisappears : 1;

	/** The actor that will activate this trigger.Set this to shooter. */
	UPROPERTY(EditAnywhere)
	AActor* TargetActor;

	UFUNCTION(BlueprintCallable)
	void SetComponents(UAudioComponent* AudioComponent);

	// Overlap
	UFUNCTION()
	void BeginOverlap(AActor* OverlappedActor, AActor* OtherActor);
};
