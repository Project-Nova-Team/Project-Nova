#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../ProjectNovaMacros.h"
#include "PatrolPoint.generated.h"

class APatrolPath;
class UArrowComponent;

UCLASS(HideCategories = (Rendering, Replication, Collision, Input, Actor, LOD, Cooking))
class PROJECTNOVA_API APatrolPoint : public AActor
{
	GENERATED_BODY()
	
public:

	friend class APatrolPath;

	APatrolPoint();

	UFUNCTION(BlueprintGetter)
	bool UseLookDirection() const { return bUseLookDirection; }

	UFUNCTION(BlueprintSetter)
	void SetUseLookDirection(const bool bValue);

protected:

	/** The Patrol Path actor that owns this object*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Patrol Point")
	APatrolPath* OwningPath;

	/** If true, the AI will turn to face in the direction of the arrow upon reaching this point*/
	UPROPERTY(EditAnywhere, BlueprintGetter = UseLookDirection, BlueprintSetter = SetUseLookDirection, Category = "Patrol Point")
	uint8 bUseLookDirection : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol Point")
	float WaitDuration;

#if WITH_EDITOR

public:

	void PostLoad() override;
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

protected:

	UPROPERTY()
	UBillboardComponent* SpriteComponent;

	UPROPERTY()
	UArrowComponent* ArrowComponent;

private:

	/** Called when the bUseLookDirection property is changed*/
	void UpdateArrowVisibility();
	
#endif
};
