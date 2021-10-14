#pragma once

#include "CoreMinimal.h"
#include "AISearchState.h"
#include "AIAgitatedState.generated.h"

UCLASS()
class UNREALPLAYGROUND_API UAIAgitatedState : public UAISearchState
{
	GENERATED_BODY()
public:
	UAIAgitatedState() { }
	~UAIAgitatedState() { }

	void Initialize(UStateMachine* StateMachine, UObject* ContextObject) override;
};
