
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Shooter.h"
#include "Camera/CameraComponent.h"
#include "VaultRaycasterComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UNREALPLAYGROUND_API UVaultRaycasterComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UVaultRaycasterComponent();

	/** check ray for vaultable surface*/
	void CheckForVault();

	/** Cast Ray from knees*/
	void CastRay();

	//The Origin of the raycast
	FVector StartLocation;
	
	AShooter* Shooter;

	UCameraComponent* Cam;

	//The length of the ray in units.
	//For more flexibility you can expose a public variable in the editor
	float RayLength = 200;

	//Parent facing direction vector
	FVector ParentDirectionVector;

	//The EndLocation of the raycast
	FVector EndLocation;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


};
