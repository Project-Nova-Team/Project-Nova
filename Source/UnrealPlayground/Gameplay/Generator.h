#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractiveObject.h"
#include "Generator.generated.h"

class AGeneratorPiece;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGeneratorPieceRepair, int32, RepairedPieceCount);

UCLASS()
class UNREALPLAYGROUND_API AGenerator : public AActor, public IInteractiveObject
{
	GENERATED_BODY()
	
public:	
	AGenerator();

	void InteractionEvent(APawn* EventSender) override;

protected:

	virtual void BeginPlay() override;

	bool CanInteract() const override { return RepairedPieces == Pieces.Num() && !bHasBeenActivated; }

	UPROPERTY(VisibleANywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)	
	TArray<AGeneratorPiece*> Pieces;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UTexture2D*> ScreenTextures;

	UPROPERTY(BlueprintAssignable)
	FGeneratorPieceRepair OnPieceRepaired;

	UFUNCTION()
	void ReceivePieceRepaired();

	int32 RepairedPieces;

	uint8 bHasBeenActivated : 1;
};
