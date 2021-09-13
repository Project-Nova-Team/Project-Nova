#include "InstructionComponent.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UInstructionComponent::UInstructionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInstructionComponent::BeginPlay()
{
	Super::BeginPlay();

	Owner = Cast<ACharacter>(GetOwner());
	Movement = Owner->GetCharacterMovement();

	if (PatrolActor != nullptr)
	{
		PatrolPath = PatrolActor->FindComponentByClass<USplineComponent>();
	}
}

FVector UInstructionComponent::GetNavigablePatrolPoint() const
{
	const int32 TargetIndex = GetNextPatrolPoint(PatrolIndex);
	const FVector RawLocation = PatrolPath->GetLocationAtSplinePoint(TargetIndex, ESplineCoordinateSpace::World);

	//TODO check if this is a valid point to try and move to? Maybe just trust designers
	//TODO line trace down to find the ground point?

	return RawLocation;
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
		//TODO figure out the best way to implement bIsRevered along a non looped track
		return 0;
	}
}

void UInstructionComponent::UpdatePatrolIndex()
{
	PatrolIndex = GetNextPatrolPoint(PatrolIndex);
}