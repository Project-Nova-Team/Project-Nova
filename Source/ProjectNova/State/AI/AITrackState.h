#pragma once

#include "CoreMinimal.h"
#include "AIInvestigationState.h"
#include "AITrackState.generated.h"

UCLASS()
class PROJECTNOVA_API UAITrackState : public UAIInvestigationState
{
	GENERATED_BODY()
public:
	UAITrackState() { }
	~UAITrackState() { }

	void OnEnter() override;
	void Initialize(UStateMachine* StateMachine, UObject* ContextObject) override;
};
