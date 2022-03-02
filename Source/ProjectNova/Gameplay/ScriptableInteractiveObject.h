#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractiveObject.h"
#include "ScriptableInteractiveObject.generated.h"

UCLASS()
class PROJECTNOVA_API AScriptableInteractiveObject : public AActor, public IInteractiveObject
{
	GENERATED_BODY()
	
public:	
	AScriptableInteractiveObject();

	bool CanInteract() const override { return bCanInteract; }

	FInteractionPrompt& GetInteractionPrompt() override { return Prompt; }

	FName& GetInteractionMappingName() override { return ActionMappingName; }

	void InteractionEvent(APawn* EventSender) override;

	void RecieveLookedAt(APawn* EventSender);

	void RecieveLookedAway(APawn* EventSender, int32 MappingIndexToRemove) override;

protected:

	/** Whether or not the object can be interacted with. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	uint8 bCanInteract : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	UStaticMeshComponent * Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FInteractionPrompt Prompt;

	/** Is set to interact by default. See Edit->ProjectSettings->Input for list of action mapping names.*/
	UPROPERTY(EditAnywhere, Category = "Interaction")
	FName ActionMappingName;
};
