#pragma once

#include "CoreMinimal.h"
#include "AISearchState.h"
#include "AIInvestigationState.generated.h"

//TODO this state name sucks. Its far too confusing with UAIInvestigate state!
//This is the parent class of Track/Follow/Investigate which involves a formal "investigation" by a particular AI
UCLASS()
class PROJECTNOVA_API UAIInvestigationState : public UAISearchState
{
	GENERATED_BODY()

public:
	void OnEnter() override;
	void OnExit() override;
};
