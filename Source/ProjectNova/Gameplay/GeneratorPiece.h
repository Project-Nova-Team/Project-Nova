#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Removable.h"
#include "GeneratorPiece.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGeneratorPieceHitEvent);

UCLASS()
class PROJECTNOVA_API AGeneratorPiece : public AActor
{
	GENERATED_BODY()
	
public:	
	AGeneratorPiece();

	UPROPERTY(BlueprintAssignable)
	FGeneratorPieceHitEvent OnGeneratorPieceRepair;

	virtual void BreakPiece();

protected:
	virtual void BeginPlay() override;

	uint8 bIsRepaired : 1;

	UPROPERTY(Category = Mesh, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;

	UPROPERTY(Category = Mesh, EditAnywhere, BlueprintReadWrite)
	UStaticMesh* FixedMesh;

	/** No need to serialize can just fetch at begin play*/
	UStaticMesh* BrokenMesh;

	UFUNCTION()
	virtual void OnOverlap(AActor* OvelappedActor, AActor* OtherActor);
};
