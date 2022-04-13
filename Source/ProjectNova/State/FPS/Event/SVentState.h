#pragma once

#include "CoreMinimal.h"
#include "../Movement/SMovementState.h"
#include "Components/SplineComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "SVentState.generated.h"

class AVent;

UENUM()
enum ECrawlDirection
{
	CD_Forward,
	CD_Back
};

UCLASS()
class PROJECTNOVA_API USVentState : public USMovementState
{
	GENERATED_BODY()

public:

	void Initialize(UStateMachine* StateMachine, UObject* ContextObject) override;
	void Tick(float DeltaTime) override;
	void OnEnter() override;
	void OnExit() override;

	/** Vent the shooter is currently in*/
	AVent* Vent;

	/** Enum denoting which direction we need to face*/
	TEnumAsByte<ECrawlDirection> CrawlDirection;

private:

	/** Progress of movement along spline between 0 and 1 of the spline*/
	float CurrentProgress;

	/** Unit length of the spline component*/
	float SplineLength;

	/** Anim instance of the shooter body*/
	class UShooterAnimInstance* AnimInstance;

private:

	/** The method that moves the player along the spline*/
	void MoveAlongSpline(float DeltaTime);

	void SetShooterRotation(float DeltaTime);
};