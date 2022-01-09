#pragma once

#include "CoreMinimal.h"
#include "AIInvestigationState.h"
#include "AIFollowState.generated.h"

UCLASS()
class PROJECTNOVA_API UAIFollowState : public UAIInvestigationState
{
	GENERATED_BODY()

public:
	UAIFollowState() { }
	~UAIFollowState() { }

	void OnEnter();
	void Initialize(UStateMachine* StateMachine, UObject* ContextObject) override;
	void Tick(const float DeltaTime) override;
};
