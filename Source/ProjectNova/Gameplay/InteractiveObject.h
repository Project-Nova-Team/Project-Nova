#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include <Runtime/Engine/Classes/GameFramework/InputSettings.h>
#include "InteractiveObject.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FInteractionEvent, APawn*);

USTRUCT(BlueprintType)
struct FInteractionPrompt
{
	GENERATED_BODY()

public:

	FInteractionPrompt() 
	{
		Prefix = TEXT("Press");
		Suffix = TEXT("to pick up ");
		Name = TEXT("object.");
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FString Prefix;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	UTexture2D* Key;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FString Suffix;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FString Name;

	//Just compare the names since there is no need to update the UI if we look at another object with an identical name
	bool operator!=(const FInteractionPrompt& rhs) const
	{
		return Name != rhs.Name;
	}
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

	virtual void ReceiveLookedAt(APawn* EventSender);

	virtual void ReceiveLookedAway(APawn* EventSender);

	virtual void InteractionEvent(APawn* EventSender);

	virtual bool CanInteract() const;

	void RemoveInputIfDisabled(APawn* EventSender);

	/** Returns the input action name this interactive object uses*/
	virtual const FName GetActionName() const;

	/** Forces classes that inherit from this to provide prompt data*/
	virtual FInteractionPrompt& GetInteractionPrompt() = 0;

	FInteractionEvent OnInteract;

	/** Index of input action binding which is used to unhook input action events*/
	int32 BindingIndex;
};
