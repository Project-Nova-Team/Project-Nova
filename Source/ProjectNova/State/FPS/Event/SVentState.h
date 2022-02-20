#pragma once

#include "CoreMinimal.h"
#include "../Movement/SProneState.h"
#include <Runtime/Engine/Classes/Components/SplineComponent.h>
#include "Components/SkeletalMeshComponent.h"
#include "SVentState.generated.h"

UCLASS()
class PROJECTNOVA_API USVentState : public USProneState
{
	GENERATED_BODY()

public:
	USVentState() {}
	~USVentState() {}

	void Tick(const float DeltaTime) override;
	void OnEnter() override;
	void Initialize(UStateMachine* StateMachine, UObject* ContextObject) override;
	void OnExit() override;

protected:

	USplineComponent* Spline;

	float Progress;
};