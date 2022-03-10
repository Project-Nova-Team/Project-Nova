#pragma once

#include "CoreMinimal.h"
#include "../Movement/SProneState.h"
#include <Runtime/Engine/Classes/Components/SplineComponent.h>
#include "Components/SkeletalMeshComponent.h"
#include "SVentState.generated.h"

class AVent;

UENUM()
enum ECrawlDirection
{
	CD_Left,
	CD_Right
};

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

	/** Spline Reference*/
	USplineComponent* Spline;

	/** Vent Reference*/
	AVent* Vent;

	/** Progress of movement along spline between 0 and the length of the spline*/
	float Progress;

	/** the length of the spline*/
	float ProgressMax;

	float StandingProgress;

	/** How fast the player moves through the vent. */
	float CrawlSpeed;

	float LerpToCrawlSpeed;

	float LerpToStandSpeed;

	float ProneOffset;

	/** Vector location at a progress*/
	FVector LocationAtDistanceAlongSpline;

	/** Lerp target vector for location*/
	FVector TargetCrawlLocation;

	/** Target Lerp rotation*/
	FRotator TargetCrawlRotation;

	FVector TargetStandingLocation;

	/** Direction at progress for rotation*/
	FVector DirectionAtDistanceAlongSpline;

	/** Enum denoting which direction we need to face*/
	ECrawlDirection CrawlDirection;

	/** is the handle finished lerping to the crawl position*/
	uint8 bIsLerpingToCrawlPosition : 1;

	uint8 bIsLerpingToStandingPosition : 1;

	/** The method that lerps the player from standing to crawl*/
	void LerpToCrawl(FVector StartPosition, FVector EndPosition, FRotator StartRotation, FRotator EndRotation);

	/** The method that lerps the player from standing to crawl*/
	void LerpToStanding(FVector StartPosition, FVector EndPosition);

	/** The method that moves the player along the spline*/
	void MoveAlongSpline(ECrawlDirection Direction, float DeltaTime);

	/** Rotates the player along the spline while moving*/
	void RotateAlongSpline();

	void LeaveCrawl(FVector StartingPosition, FVector EndPosition);
};