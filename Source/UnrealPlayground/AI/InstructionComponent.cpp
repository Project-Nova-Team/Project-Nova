#include "InstructionComponent.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UInstructionComponent::UInstructionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInstructionComponent::Initialize()
{
	Owner = Cast<ACharacter>(GetOwner());
	Movement = Owner->GetCharacterMovement();

	if (PatrolActor != nullptr)
	{
		PatrolPath = PatrolActor->FindComponentByClass<USplineComponent>();
	}
}

FVector UInstructionComponent::GetNavigablePatrolPoint() const
{
	return PatrolPath->GetLocationAtSplinePoint(PatrolIndex, ESplineCoordinateSpace::World);
}

int32 UInstructionComponent::GetNextPatrolPoint(const int32 CurrentPoint) const
{
	const int32 PointCount = PatrolPath->GetNumberOfSplinePoints();

	if (PatrolPath->IsClosedLoop())
	{
		//We are at the last point in the loop, move back to the start
		if (CurrentPoint == PointCount - 1)
		{
			return 0;
		}

		//Progress to the next point
		else
		{
			return CurrentPoint + 1;
		}
	}

	else
	{
		if (!bIsReversedOnPath)
		{
			//We are at the last point, turn around
			if (CurrentPoint == PointCount - 1)
			{
				return PointCount - 2;
			}

			//Just advanced forwards
			else
			{
				return CurrentPoint + 1;
			}
		}

		else
		{
			//We reached the beginning point, turn around
			if (CurrentPoint == 0)
			{
				return 1;
			}

			//Move towards the first point
			else
			{
				return CurrentPoint - 1;
			}
		}
	}
}

void UInstructionComponent::UpdatePatrolIndex()
{
	const int32 Previous = PatrolIndex;
	PatrolIndex = GetNextPatrolPoint(PatrolIndex);

	if (!bIsReversedOnPath && PatrolIndex < Previous)
	{
		bIsReversedOnPath = true;
	}

	else if (bIsReversedOnPath && PatrolIndex > Previous)
	{
		bIsReversedOnPath = false;
	}
}

bool UInstructionComponent::HasPatrolPath() const
{
	return PatrolPath != nullptr;
}

//Intellisense is lying, this works just fine...
void UInstructionComponent::OnStimulus(AActor* Invoker, FAIStimulus Stimulus)
{
	//When in combat the AI doesn't care about stimulus, it just wants to attack
	if (bIsInCombat)
	{
		return;
	}
}