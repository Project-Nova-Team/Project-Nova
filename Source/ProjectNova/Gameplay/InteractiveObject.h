#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractiveObject.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FInteractionEvent, APawn*);

USTRUCT(BlueprintType)
struct FInteractionPrompt
{
	GENERATED_BODY()

public:

	FInteractionPrompt() 
		: Prefix(NSLOCTEXT("Interaction","Press", "Press "))
		, Suffix(NSLOCTEXT("Interaction", "topickup", "to pick up "))
		, Name(NSLOCTEXT("Interaction", "Object", "Object"))
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FText Prefix;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	UTexture2D* Key;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FText Suffix;

	/** Display text, what actually appears on screen*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FText Name;
};

UINTERFACE(MinimalAPI)
class UInteractiveObject : public UInterface
{
	GENERATED_BODY()
};

class PROJECTNOVA_API IInteractiveObject
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Interact"))
	void BlueprintInteract(APawn* EventSender);

	virtual void InteractionEvent(APawn* EventSender) { OnInteract.Broadcast(EventSender); }

	FInteractionEvent OnInteract;

	virtual bool CanInteract() const { return true; }

	/** Forces classes that inherit from this provide prompt data*/
	virtual const FInteractionPrompt& GetInteractionPrompt() const = 0;
};