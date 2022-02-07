#pragma once

#include "CoreMinimal.h"
#include "GeneratorPiece.h"
#include "InteractiveObject.h"
#include "InteractableGeneratorPiece.generated.h"

UCLASS()
class PROJECTNOVA_API AInteractableGeneratorPiece : public AGeneratorPiece, public IInteractiveObject
{
	GENERATED_BODY()

public:
	void InteractionEvent(APawn* EventSender) override;

	bool CanInteract() const override { return !bIsRepaired; }

	FInteractionPrompt& GetInteractionPrompt() override { return Prompt; }

	FName& GetInteractionMappingName() override { return ActionMappingName; }

	void RecieveLookedAt(APawn* EventSender);

	void RecieveLookedAway(APawn* EventSender, int32 MappingIndexToRemove) override;

protected:

	virtual void BeginPlay() override;

	UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "Interactable")
	FInteractionPrompt Prompt;

	/** Is set to interact by default. See Edit->ProjectSettings->Input for list of action mapping names.*/
	UPROPERTY(EditAnywhere, Category = "Interaction")
	FName ActionMappingName;
};
