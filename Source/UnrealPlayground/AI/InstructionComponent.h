#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Perception/AIPerceptionTypes.h"
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
	bool HasPatrolPath() const;

	/**
	 * Event invoked when this controller senses a disturbance in the environment
	 *
	 * @param	Invoker				The actor who caused the stimulus
	 * @param	Stimulus			The actual stimulus that the controller picked up
	 */
	UFUNCTION()
	void OnStimulus(AActor* Invoker, FAIStimulus Stimulus);

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

	

	///Begin search

	/** True if the behavior tree is in combat mode meaning we ignore any stimulus*/
	uint8 bIsInCombat : 1;

	/** True if the AI is currently following the player position in an effort to reveal them*/
	uint8 bIsFollowing : 1;

	/** True if the AI is moving to the player's last known location*/
	uint8 bIsTracking : 1;

	/** True or not the AI is moving to a previous stimulus location*/
	uint8 bIsInvestigating : 1;

private:
	ACharacter* Owner;
	UCharacterMovementComponent* Movement;
	UCombatComponent* Combat;
};
