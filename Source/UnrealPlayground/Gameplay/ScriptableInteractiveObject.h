#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractiveObject.h"
#include "ScriptableInteractiveObject.generated.h"

UCLASS()
class UNREALPLAYGROUND_API AScriptableInteractiveObject : public AActor, public IInteractiveObject
{
	GENERATED_BODY()
	
public:	
	AScriptableInteractiveObject();

	bool CanInteract() const override { return bCanInteract; }

	FInteractionPrompt& GetInteractionPrompt() override { return Prompt; }

	void InteractionEvent(APawn* EventSender) override;

protected:

	/** Whether or not the object can be interacted with. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	uint8 bCanInteract : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	UStaticMeshComponent * Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FInteractionPrompt Prompt;
};
