#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractiveObject.h"
#include "Generator.generated.h"

class AGeneratorPiece;

DECLARE_MULTICAST_DELEGATE(FGeneratorInteractedEvent);

UCLASS()
class UNREALPLAYGROUND_API AGenerator : public AActor, public IInteractiveObject
{
	GENERATED_BODY()
	
public:	
	AGenerator();

	void InteractionEvent(APawn* EventSender) override;

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleANywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;

	// How many times the player has fixed a broken piece
	int HitCount;

	// Increases hit count. 3 == fixed
	void IterateHitCount();

	void CheckIfFixed();

	UPROPERTY(EditAnywhere)	
	TArray<AGeneratorPiece*> BrokenPieces;

	// delegate
	FGeneratorInteractedEvent OnGeneratorInteracted;
};
