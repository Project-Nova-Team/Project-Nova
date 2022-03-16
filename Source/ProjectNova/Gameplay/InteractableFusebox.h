// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractiveObject.h"
#include "InteractableFusebox.generated.h"

class UAnimInstance;

UCLASS()
class PROJECTNOVA_API AInteractableFusebox : public AActor, public IInteractiveObject
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractableFusebox();

	bool CanInteract() const override { return bCanInteract; }

	FInteractionPrompt& GetInteractionPrompt() override { return Prompt; }

	void InteractionEvent(APawn* EventSender) override;

	UPROPERTY(Category = Mesh, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;

	void SetCanInteract(bool Status);

protected:

	/** Whether or not the object can be interacted with. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	uint8 bCanInteract : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FInteractionPrompt Prompt;

	/** Is set to interact by default. See Edit->ProjectSettings->Input for list of action mapping names.*/
	UPROPERTY(EditAnywhere, Category = "Interaction")
	FName ActionMappingName;

	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimInstance* Anim;
};
