#include "AICell.h"
#include "AIBase.h"
#include "Components/BrushComponent.h"
#include "../Player/Shooter.h"
#include "../State/AI/AIStateMachine.h"
#include "Sense.h"

AAICell::AAICell()
{
	PrimaryActorTick.bCanEverTick = true;
	GetBrushComponent()->SetCollisionProfileName("OverlapOnlyPawn");
	GetBrushComponent()->SetGenerateOverlapEvents(true);
	GetBrushComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECollisionResponse::ECR_Ignore);
}

void AAICell::BeginPlay()
{
	Super::BeginPlay();
	Player->OnMakeNoise.AddDynamic(this, &AAICell::RegisterAudioStimulus);	
}

void AAICell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//GEngine->AddOnScreenDebugMessage(50, 1.f, FColor::Red, FString::FromInt(AgressorCount));
	//GEngine->AddOnScreenDebugMessage(51, 1.f, FColor::Blue, FString::FromInt(InvestigationCount));
}

void AAICell::SetAIUnits()
{	
	AIUnits.Reset();

	TArray<FHitResult> Hits;	
	const FVector ActorLocation = GetActorLocation();
	const FQuat ActorRotation = GetActorRotation().Quaternion();
	const FVector ActorSize = GetBounds().BoxExtent;
	const FCollisionShape TestBox = FCollisionShape::MakeBox(ActorSize);
	FCollisionQueryParams Params;
	Params.bIgnoreBlocks = true;
	Params.bIgnoreTouches = false;
	GetWorld()->SweepMultiByProfile(Hits, ActorLocation, ActorLocation, ActorRotation, "OverlapOnlyPawn", TestBox, Params);

	for (FHitResult Hit : Hits)
	{
		AActor* const HitActor = Hit.GetActor();

		if (HitActor != nullptr && HitActor->IsA(AAIBase::StaticClass()))
		{
			AAIBase* const AsAI = Cast<AAIBase>(HitActor);
			AAIBaseController* const Controller = Cast<AAIBaseController>(AsAI->GetController());

			AIUnits.Add(Controller);
			Controller->SetCell(this);
			Controller->SetTarget(Player);
			AsAI->SetPlayer(Player);

			Controller->OnInvestigation.AddDynamic(this, &AAICell::RegisterInvestigator);
			Controller->OnAggression.AddDynamic(this, &AAICell::RegisterAggressor);
			Controller->OnHeardSound.AddDynamic(this, &AAICell::SignalCellStimulus);

			if (AsAI->bStartsInActive)
			{
				Controller->SetAIActive(false, false);
			}
		}
	}
}

AAIBaseController* AAICell::GetNearestAI(const FVector& Point)
{
	AAIBaseController* NearestAI = nullptr;
	float CurrentClosestDistance = 1e10f; // big enough

	for (AAIBaseController* AI : AIUnits)
	{
		if (AI->IsDead() || AI->IsInactive())
		{
			continue;
		}

		const float SqrDist = (AI->GetPawn()->GetActorLocation() - Point).SizeSquared();

		if (SqrDist < CurrentClosestDistance)
		{
			CurrentClosestDistance = SqrDist;
			NearestAI = AI;
		}
	}

	return NearestAI;
}

void AAICell::SignalCellStimulus(const FVector StimulusSource, const float Strength)
{
	//This stimulus has already been registered by another AI, skip it
	if (!CurrentStimulusLocation.IsNearlyZero())
	{
		return;
	}
	CurrentStimulusLocation = StimulusSource;

	AAIBaseController* const NearestAI = GetNearestAI(StimulusSource);
	
	//The cell either has no AI or all the AI are dead
	if (NearestAI == nullptr)
	{
		return;
	}

	NearestAI->DetermineSearch(StimulusSource, Strength);
}

void AAICell::RegisterInvestigator(bool StartingInvestigation)
{
	const int Change = StartingInvestigation ? 1 : -1;
	InvestigationCount += Change;

	//No AI in this cell were previously investigating
	if (StartingInvestigation && InvestigationCount == 1)
	{
		//Lets let every AI be able to turn towards the source
		for (AAIBaseController* AI : AIUnits)
		{
			AI->SetInvestigationLocation(CurrentStimulusLocation);		
		}
		SetAllUnitStates("Agitated", true);
	}

	//All Investigations in this cell have stopped, stop agitation
	if (InvestigationCount == 0 && AgressorCount == 0)
	{
		SetAllUnitStates("Patrol");
	}
}

void AAICell::RegisterAggressor(bool StaringAggression)
{
	const int Change = StaringAggression ? 1 : -1;
	AgressorCount += Change;

	//No AI in this cell were previously aggressive, alert all cell mates to attack
	if (StaringAggression && AgressorCount == 1)
	{
		SetAllUnitStates("Attack");
	}

	//All aggressors in this cell have stopped, stop attack and start agitation
	if (AgressorCount == 0)
	{
		//Hacky way to get the target
		AAIBaseController* NearestAI = GetNearestAI(AIUnits[0]->GetTargetLocation());

		if (NearestAI != nullptr)
		{
			SetAllUnitStates("Agitated", false, NearestAI);
			NearestAI->SetState("Investigate");
		}
	}
}

void AAICell::RegisterAudioStimulus(FVector Location, float Volume)
{
	CurrentStimulusLocation = FVector::ZeroVector;

	if (Volume > 0 && AgressorCount == 0)
	{
		UAISense_Hearing::ReportNoiseEvent(GetWorld(), Location, Volume, Player);
	}
}

void AAICell::SetAllUnitStates(const FString NewState, const bool bIgnoreLower, AAIBaseController* Ignore)
{
	for (AAIBaseController* AI : AIUnits)
	{
		if (AI->IsDead() || AI->IsInactive() || AI == Ignore)
		{
			continue;
		}

		//If we are ignoring lower priority states we need to check if the new state is higher
		if (bIgnoreLower)
		{
			UAIStateMachine* AIStateMachine = AI->GetStateMachine();
			const uint8 CurrentWeight = AIStateMachine->GetActiveWeight();
			const uint8 NewWeight = AIStateMachine->GetWeightAtKey(NewState);

			if (CurrentWeight >= NewWeight)
			{
				continue;
			}
		}

		AI->SetState(NewState);
	}
}