#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractiveObject.generated.h"

UINTERFACE(MinimalAPI)
class UInteractiveObject : public UInterface
{
	GENERATED_BODY()
};

class UNREALPLAYGROUND_API IInteractiveObject
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnInteract"))
	void BlueprintInteract(APawn* EventSender);

	virtual void InteractionEvent(APawn* EventSender) = 0;

	virtual bool CanInteract() const { return true; }
};
