#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InstructionComponent.generated.h"

class ACharacter;
class UCharacterMovementComponent;
class USplineComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREALPLAYGROUND_API UInstructionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInstructionComponent();

protected:

	virtual void BeginPlay() override;

	/** Actor in the level that contains a spline component the AI will patrol*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Patrol")
	AActor* PatrolActor;

	/** A spline representing the path this AI will try to patrol*/	
	USplineComponent* PatrolPath;

	/** The index point of the patrol path the AI is at*/
	UPROPERTY(BlueprintReadOnly)
	int32 PatrolIndex;

	/** Returns the position of the next patrol path waypoint the AI will try to navigate towards*/
	UFUNCTION(BlueprintCallable)
	FVector GetNavigablePatrolPoint() const;

	/** Returns the spline point index that comes after StartingPoint*/
	UFUNCTION(BlueprintCallable)
	int32 GetNextPatrolPoint(const int32 StartingPoint) const;

	UFUNCTION(BlueprintCallable)
	void UpdatePatrolIndex();

private:
	ACharacter* Owner;
	UCharacterMovementComponent* Movement;
};
