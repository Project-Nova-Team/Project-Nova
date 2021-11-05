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
	uint8 bCanOnlyBeInteractedWithOnce : 1;

	virtual void InteractionEvent(APawn* EventSender) = 0;

	uint8 bCanInteract : 1;

};
