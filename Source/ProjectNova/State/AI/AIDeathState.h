#pragma once

#include "CoreMinimal.h"
#include "AIState.h"
#include "AIDeathState.generated.h"


UCLASS()
class PROJECTNOVA_API UAIDeathState : public UAIState
{
	GENERATED_BODY()

public: 
	UAIDeathState() { }
	~UAIDeathState() { }
};
