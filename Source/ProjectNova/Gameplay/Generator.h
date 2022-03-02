#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractiveObject.h"
#include "Generator.generated.h"

class AGeneratorPiece;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGeneratorPieceRepair, int32, RepairedPieceCount);

UCLASS()
class PROJECTNOVA_API AGenerator : public AActor, public IInteractiveObject
{
	GENERATED_BODY()
	
public:	
	AGenerator();

	void InteractionEvent(APawn* EventSender) override;

	FInteractionPrompt& GetInteractionPrompt() override { return Prompt; }

	FName& GetInteractionMappingName() override { return ActionMappingName; }

	bool CanInteract() const override { return RepairedPieces == Pieces.Num() && !bHasBeenActivated; }

	void RecieveLookedAt(APawn* EventSender) override;

	void RecieveLookedAway(APawn* EventSender, int32 MappingIndexToRemove) override;

protected:

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FInteractionPrompt Prompt;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)	
	TArray<AGeneratorPiece*> Pieces;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UTexture2D*> ScreenTextures;

	UPROPERTY(BlueprintAssignable)
	FGeneratorPieceRepair OnPieceRepaired;

	UFUNCTION()
	virtual void ReceivePieceRepaired();

	UFUNCTION(BlueprintCallable)
	void BreakGenerator();

	int32 RepairedPieces;

	uint8 bHasBeenActivated : 1;

	/** Is set to interact by default. See Edit->ProjectSettings->Input for list of action mapping names.*/
	UPROPERTY(EditAnywhere, Category = "Interaction")
	FName ActionMappingName;
};
