#pragma once

#include "CoreMinimal.h"
#include "SEventState.h"
#include "SCutsceneState.generated.h"


UCLASS()
class PROJECTNOVA_API USCutsceneState : public USEventState
{
	GENERATED_BODY()
	
public:

	void OnEnter() override;
	void OnExit() override;
};
