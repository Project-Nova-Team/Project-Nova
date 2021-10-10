#pragma once

#include "CoreMinimal.h"
#include "AIState.h"
#include "AIDeathState.generated.h"


UCLASS()
class UNREALPLAYGROUND_API UAIDeathState : public UAIState
{
	GENERATED_BODY()

public: 
	UAIDeathState() { }
	~UAIDeathState() { }

	void OnEnter() override { AI->CurrentState = GetClass()->GetFName().GetPlainNameString(); }
};
