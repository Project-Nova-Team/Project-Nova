// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Removable.h"
#include "GeneratorPiece.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGeneratorPieceHitEvent);

UCLASS()
class UNREALPLAYGROUND_API AGeneratorPiece : public AActor
{
	GENERATED_BODY()
	
public:	
	AGeneratorPiece();

	UPROPERTY(BlueprintAssignable)
	FGeneratorPieceHitEvent OnGeneratorPieceRepair;

protected:
	virtual void BeginPlay() override;

	uint8 bIsRepaired : 1;

	UPROPERTY(Category = Mesh, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;

	UPROPERTY(Category = Mesh, EditAnywhere, BlueprintReadWrite)
	UStaticMesh* FixedMesh;

	UFUNCTION()
	void OnOverlap(AActor* OvelappedActor, AActor* OtherActor);
};
