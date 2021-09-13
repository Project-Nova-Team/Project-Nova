#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InstructionComponent.generated.h"

class ACharacter;
class UCharacterMovementComponent;
class UCombatComponent;
class USplineComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREALPLAYGROUND_API UInstructionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInstructionComponent();

	/** Performs component set up*/
	void Initialize();

	/** Returns true if this instruction have a patrol path attached to it which is assigned in the editor*/
	bool HasPatrolPath();

protected:
	/** Actor in the level that contains a spline component the AI will patrol*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Patrol")
	AActor* PatrolActor;

	/** A spline representing the path this AI will try to patrol*/	
	USplineComponent* PatrolPath;

	/** Whether or not the AI will walk in reverse order of the points when on a non looped path*/
	uint8 bIsReversedOnPath : 1;

	/** The index point of the patrol path the AI is at*/
	UPROPERTY(BlueprintReadOnly)
	int32 PatrolIndex;

	/** Returns the position of the next patrol path waypoint the AI will try to navigate towards*/
	UFUNCTION(BlueprintCallable)
	FVector GetNavigablePatrolPoint() const;

	/** Returns the spline point index that comes after StartingPoint*/
	UFUNCTION(BlueprintCallable)
	int32 GetNextPatrolPoint(const int32 StartingPoint) const;

	/** Sets the patrol index to the next point on the patrol path*/
	UFUNCTION(BlueprintCallable)
	void UpdatePatrolIndex();

private:
	ACharacter* Owner;
	UCharacterMovementComponent* Movement;
	UCombatComponent* Combat;
};
