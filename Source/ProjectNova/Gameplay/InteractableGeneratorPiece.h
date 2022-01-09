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

protected:

	virtual void BeginPlay() override;

	UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "Interactable")
	FInteractionPrompt Prompt;
	
};
