#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../ProjectNovaMacros.h"
#include "Components/SplineComponent.h"
#include "PatrolPath.generated.h"

UENUM()
enum EPathMode
{
	Track,
	Loop,
	Random 
};

USTRUCT(BlueprintType)
struct FPatrolPoint
{
	GENERATED_BODY()

public:
	FPatrolPoint() { }
	FPatrolPoint(float Time, int32 PointIndex, USplineComponent* Spline);

	UPROPERTY(BlueprintReadOnly)
	float WaitTime;

	UPROPERTY(BlueprintReadOnly)
	int32 PointIndex;

	UPROPERTY(BlueprintReadOnly)
	USplineComponent* SplineComponent;
};

UCLASS(HideCategories=(Rendering, Replication, Collision, Input, Actor, LOD, Cooking))
class PROJECTNOVA_API APatrolPath : public AActor
{
	GENERATED_BODY()

public:

	APatrolPath();

	/** Provided an AI's current patrol index, returns the next travel point. This should only be called for paths in Track/Loop mode*/
	UFUNCTION(BlueprintCallable, Category = "Patrol Path")
	FPatrolPoint GetNextPoint(const int32 CurrentPointIndex, UPARAM(ref)bool& bReversed, int32& OutPointIndex);

	/** Given a collection of remembered points, select a random available point NOT in the collection*/
	UFUNCTION(BlueprintCallable, Category = "Patrol Path")
	FPatrolPoint GetRandomPoint(UPARAM(ref)TArray<int32>& RememberedPoints, int32 MemoryLimit);

protected:

	/** Describes behavior for AI assigned to this path*/
	UPROPERTY(EditAnywhere, BlueprintGetter = GetPathMode, BlueprintSetter = SetPathMode, Category = "Patrol Path")
	TEnumAsByte<EPathMode> Mode;

	UFUNCTION(BlueprintGetter)
	EPathMode GetPathMode() const { return Mode; }

	UFUNCTION(BlueprintSetter)
	void SetPathMode(const EPathMode NewMode);

#if WITH_EDITORONLY_DATA

public:

	void PostEditChangeProperty(FPropertyChangedEvent& Event) override;

	/** Updates spline component when this gets changed*/
	void SetPathModeImpl();

#endif //WITH_EDITOR

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USplineComponent* SplineComponent;

	/** Number of seconds the AI will wait at each patrol point. The index of the time correlates to the point index along the spline*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol Path")
	TArray<float> WaitTimes;


};