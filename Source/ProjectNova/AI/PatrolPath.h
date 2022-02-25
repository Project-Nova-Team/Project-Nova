#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../ProjectNovaMacros.h"
#include "PatrolPath.generated.h"

class APatrolPoint;
class USplineComponent;

UENUM()
enum EPathMode
{
	Track,
	Loop,
	Random 
};

UCLASS(HideCategories=(Rendering, Replication, Collision, Input, Actor, LOD, Cooking))
class PROJECTNOVA_API APatrolPath : public AActor
{
	GENERATED_BODY()

public:

	APatrolPath();

	/** Provided an AI's current patrol index, returns the next travel point. This should only be called for paths in Track/Loop mode*/
	UFUNCTION(BlueprintCallable, Category = "Patrol Path")
	APatrolPoint* GetNextPoint(const int32 CurrentPointIndex, UPARAM(ref)bool& bReversed, int32& OutPointIndex);

	/** Given a collection of remembered points, select a random available point NOT in the collection*/
	UFUNCTION(BlueprintCallable, Category = "Patrol Path")
	APatrolPoint* GetRandomPoint(UPARAM(ref)TArray<APatrolPoint*>& RememberedPoints, int32 MemoryLimit);

protected:

	/** Describes behavior for AI assigned to this path*/
	UPROPERTY(EditAnywhere, BlueprintGetter = GetPathMode, BlueprintSetter = SetPathMode, Category = "Patrol Path")
	TEnumAsByte<EPathMode> Mode;

	/** The points belonging to */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Patrol Path")
	TArray<APatrolPoint*> Points;

	UFUNCTION(BlueprintGetter)
	EPathMode GetPathMode() const { return Mode; }

	UFUNCTION(BlueprintSetter)
	void SetPathMode(const EPathMode NewMode);

#if WITH_EDITORONLY_DATA

public:

	void Tick(float DeltaSeconds) override;

	/** Enables tick during edit time only*/
	virtual bool ShouldTickIfViewportsOnly() const override;

	/** Creates a new PatrolPoint actor and adds it to the Points collection*/
	FReply AddPoint();

	/** Binds engine delegates for necessary cleanup*/
	void PostLoad() override;

	void PostEditChangeProperty(FPropertyChangedEvent& Event) override;

	/** Updates spline component when this gets changed*/
	void SetPathModeImpl();

protected:

	UPROPERTY()
	UBillboardComponent* SpriteComponent;

	UPROPERTY()
	USplineComponent* SplineComponent;

	/** Event handle for cleanup*/
	FDelegateHandle Handle;

	/** Called when an object is deleted in the Editor. Performs necessary cleanup on relevant objects*/
	void ReceiveEditorDeletedActor(AActor* Actor);

#endif //WITH_EDITOR
};