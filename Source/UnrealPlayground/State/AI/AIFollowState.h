#pragma once

#include "CoreMinimal.h"
#include "AISearchState.h"
#include "AIFollowState.generated.h"

UCLASS()
class UNREALPLAYGROUND_API UAIFollowState : public UAISearchState
{
	GENERATED_BODY()

public:
	UAIFollowState() { }
	~UAIFollowState() { }

	void OnEnter();
	void Initialize(UStateMachine* StateMachine, UObject* ContextObject) override;
	void Tick(const float DeltaTime) override;
};
