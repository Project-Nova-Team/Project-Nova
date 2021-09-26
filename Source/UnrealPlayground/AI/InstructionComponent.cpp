#include "InstructionComponent.h"
#include "Components/SplineComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

UInstructionComponent::UInstructionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	MinimumPropagationDistance = 10000.f;
	MinimumIntensityThreshold = 10.e-12f;
	MinimumHearingThreshold = 10.f;
	AnnoyanceThreshold = 5;

	TrackScore = 100.f;
	FollowScore = 200.f;
	FollowDuration = 15.f;
	AttackScore = 300.f;

	NoSightAttackDuration = 20.f;
}

void UInstructionComponent::Initialize(UBlackboardComponent* InitBlackboard)
{
	Blackboard = InitBlackboard;
	Blackboard->SetValueAsObject("Instruction", this);

	PatrolPath = GetPatrolPath();
	const bool bHasPatrolPath = PatrolPath != nullptr;

	if (!bHasPatrolPath && Home.IsNearlyZero())
	{
		Home = GetOwner()->GetActorLocation();
	}

	Blackboard->SetValueAsBool("bHasPatrolPath", bHasPatrolPath);
	Blackboard->SetValueAsVector("HomeLocation", Home);
}

void UInstructionComponent::SetState(const EInstructionState NewState)
{
	if (NewState == State)
	{
		return;
	}

	State = NewState;
	CurrentAnnoyance = 0;

	if (State != EInstructionState::Search)
	{
		//TODO we may want to set every search mode key to false here
		SearchMode = ESearchMode::NotSearching;
		CurrentInvestigationScore = 0;
	}

	OnInstructionStateChange.Broadcast(State);
}

void UInstructionComponent::SetSearchMode(const ESearchMode NewMode)
{
	if (NewMode == SearchMode)
	{
		return;
	}

	SearchMode = NewMode;
	CurrentAnnoyance = 0;

	switch (NewMode)
	{
	case ESearchMode::NotSearching:
		CurrentInvestigationScore = 0;
		Blackboard->SetValueAsBool("bIsInvestigating", false);
		Blackboard->SetValueAsBool("bIsTracking", false);
		Blackboard->SetValueAsBool("bIsFollowing", false);
		break;
	case ESearchMode::Follow:
		FollowBeginTime = GetWorld()->GetTimeSeconds();
		Blackboard->SetValueAsBool("bIsInvestigating", false);
		Blackboard->SetValueAsBool("bIsTracking", false);
		Blackboard->SetValueAsBool("bIsFollowing", true);
		Blackboard->SetValueAsVector("MoveLocation", Player->GetActorLocation());
		break;
	case ESearchMode::Track:
		Blackboard->SetValueAsBool("bIsInvestigating", false);
		Blackboard->SetValueAsBool("bIsTracking", true);
		Blackboard->SetValueAsBool("bIsFollowing", false);
		Blackboard->SetValueAsVector("MoveLocation", Player->GetActorLocation());
		break;
	case ESearchMode::Investigate:
		Blackboard->SetValueAsBool("bIsInvestigating", true);
		Blackboard->SetValueAsBool("bIsTracking", false);
		Blackboard->SetValueAsBool("bIsFollowing", false);
		Blackboard->SetValueAsVector("MoveLocation", AudioInvestigationLocation);
		break;		
	}
}

void UInstructionComponent::OnStimulus(AActor* Invoker, FAIStimulus Stimulus)
{
	//Trim some excess so its more clear what sense is being affected
	FString Type = Stimulus.Type.Name.GetPlainNameString();	
	Type.RemoveFromStart("Default__AISense_");

	if (Type == "Touch" || Type == "Damage")
	{
		SetState(EInstructionState::Attack);

		if (!bHasVisionOfPlayer)
		{
			LastAggressionTime = GetWorld()->GetTimeSeconds();
		}

		return;
	}

	if (Type == "Sight")
	{	
		bHasVisionOfPlayer = Stimulus.WasSuccessfullySensed();

		if (bHasVisionOfPlayer)
		{		
			SetState(EInstructionState::Attack);
#if WITH_EDITOR
			if (bDebug)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "AI saw player");
			}
#endif			
		}

		else
		{
#if WITH_EDITOR
			if (bDebug)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "AI lost vision of player");
			}
#endif	
			LastAggressionTime = GetWorld()->GetTimeSeconds();
		}

		return;
	}

	//AI randomly hears audio of strength 1? discard these.
	//We don't care about audio stimulus if we're in attack mode
	if (Stimulus.Strength <= 1 || State == EInstructionState::Attack)
	{
		return;
	}

	if (State == EInstructionState::Patrol)
	{
		const float StimulusScore = CalculateAudioScore(Stimulus.StimulusLocation, Stimulus.Strength);
		DetermineSearchFromPatrol(Stimulus.StimulusLocation, StimulusScore);
	}

	else if (State == EInstructionState::Search)
	{
		const float StimulusScore = CalculateAudioScore(Stimulus.StimulusLocation, Stimulus.Strength);
		DetermineSearchFromSearch(Stimulus.StimulusLocation, StimulusScore);
	}
}

USplineComponent* UInstructionComponent::GetPatrolPath() const
{
	if (PatrolActor == nullptr)
	{
		return nullptr;
	}
	 
	return PatrolActor->FindComponentByClass<USplineComponent>();
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

float UInstructionComponent::CalculateAudioScore(const FVector SourceLocation, float Volume) const
{
	// Calculate the relative sound pressure from the source using an inverse linear relationship
	// Essentially, everytime the distance from the minimum propagation distance (100 units) doubles
	// The relative intensity of the audio source is cut in half. Distance to relative volume are as follows
	// 100 units - Full volume
	// 200 units - 1/2 volume
	// 400 units - 1/4 volume
	// We use decibels here so design has an easier time understanding how loud a sound really was to the listener

	const float SquareDistance = FVector::DistSquared(SourceLocation, GetOwner()->GetActorLocation());
	float RawIntensity = MinimumIntensityThreshold * FMath::Pow(10.f, Volume / 10.f);

	if(SquareDistance > MinimumPropagationDistance)
	{
		RawIntensity = RawIntensity * MinimumPropagationDistance / SquareDistance;	
	}

	const float OverNaught = RawIntensity / MinimumIntensityThreshold;
	const float Decibel = 10.f * FMath::LogX(10, OverNaught);

#if WITH_EDITOR
	if (bDebug)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Heard sound of volume: " + FString::FromInt(Decibel) + " dB");
	}
#endif

	return Decibel;
}

void UInstructionComponent::DetermineSearchFromPatrol(const FVector SourceLocation, const float IncomingScore)
{
	CurrentInvestigationScore = IncomingScore;
	AudioInvestigationLocation = SourceLocation;

	if (IncomingScore >= FollowScore)
	{
		SetSearchMode(ESearchMode::Follow);
	}

	else if (IncomingScore >= TrackScore)
	{
		SetSearchMode(ESearchMode::Track);		
	}

	else
	{	
		SetSearchMode(ESearchMode::Investigate);
	}

	SetState(EInstructionState::Search);
}

void UInstructionComponent::DetermineSearchFromSearch(const FVector SourceLocation, const float IncomingScore)
{
	if (IncomingScore > CurrentInvestigationScore)
	{
		CurrentInvestigationScore = IncomingScore;
		AudioInvestigationLocation = SourceLocation;
	}

	CurrentAnnoyance++;

	if (IncomingScore >= FollowScore)
	{
		SetSearchMode(ESearchMode::Follow);
	}

	else if (IncomingScore >= TrackScore)
	{
		SetSearchMode(ESearchMode::Track);
	}

	else
	{	
		SetSearchMode(ESearchMode::Investigate);
	}

	//The noise didnt change the state and we've been annoyed too many times. Increase search aggression
	if (CurrentAnnoyance > AnnoyanceThreshold)
	{
		switch (SearchMode)
		{
		case ESearchMode::Follow:
			SetState(EInstructionState::Attack);
			break;
		case ESearchMode::Track:
			SetSearchMode(ESearchMode::Follow);
			break;
		case ESearchMode::Investigate:
			SetSearchMode(ESearchMode::Track);
		}
	}
}

void UInstructionComponent::TickFollow(const float CurrentTime)
{
	if (CurrentTime - FollowBeginTime >= FollowDuration)
	{
		SetSearchMode(ESearchMode::Investigate);
	}

	else
	{
		Blackboard->SetValueAsVector("MoveLocation", Player->GetActorLocation());
	}
}

void UInstructionComponent::TickAttack(const float CurrentTime)
{
	const FVector PlayerLoc = Player->GetActorLocation();
	Blackboard->SetValueAsVector("MoveLocation", PlayerLoc);

	//The player escaped sight of the AI long enough, enter investigation mode
	if (!bHasVisionOfPlayer && CurrentTime >= LastAggressionTime + NoSightAttackDuration)
	{
		CurrentInvestigationScore = 0;
		AudioInvestigationLocation = PlayerLoc;

		SetState(EInstructionState::Search);		
		SetSearchMode(ESearchMode::Investigate);
	}
}