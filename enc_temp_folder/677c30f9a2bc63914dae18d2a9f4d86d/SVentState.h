#pragma once

#include "CoreMinimal.h"
#include "../Movement/SProneState.h"
#include <Runtime/Engine/Classes/Components/SplineComponent.h>
#include "Components/SkeletalMeshComponent.h"
#include "SVentState.generated.h"

class AVent;

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

	AVent* Vent;

	uint8 bIsDirectionPositive : 1;

	FVector LocationAlongSpline;

	float SpeedAlongSpline;

	float ProgressMax;

	void MoveToCrawlPosition(FVector StartPosition, FVector EndPosition, FRotator StartRotation, FRotator EndRotation);

	uint8 bMovingToCrawl : 1;

	float time;
};