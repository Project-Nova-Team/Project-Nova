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
	virtual void InteractionEvent(const APawn* EventSender) = 0;

	virtual void SetInteractiveObjectHidden(bool ActiveState) = 0;
};
