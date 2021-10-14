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
	SetAIUnits();
}

void AAICell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	GEngine->AddOnScreenDebugMessage(50, 1.f, FColor::Red, FString::FromInt(AgressorCount));
	GEngine->AddOnScreenDebugMessage(51, 1.f, FColor::Blue, FString::FromInt(InvestigationCount));
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
		if (Hit.Actor != nullptr && Hit.Actor->IsA(AAIBase::StaticClass()))
		{
			AAIBase* const AsAI = Cast<AAIBase>(Hit.Actor);
			AAIBaseController* const Controller = Cast<AAIBaseController>(AsAI->GetController());

			AIUnits.Add(Controller);
			Controller->SetCell(this);
			Controller->SetTarget(Player);
			AsAI->SetPlayer(Player);
			AsAI->GetMesh()->SetCollisionProfileName("HitScan");

			Controller->OnInvestigation.AddDynamic(this, &AAICell::RegisterInvestigator);
			Controller->OnAggression.AddDynamic(this, &AAICell::RegisterAggressor);
			Controller->OnHeardSound.AddDynamic(this, &AAICell::SignalCellStimulus);
		}
	}
}

void AAICell::SignalCellStimulus(const FVector StimulusSource, const float Strength)
{
	//This stimulus has already been registered by another AI, skip it
	if (!CurrentStimulusLocation.IsNearlyZero())
	{
		return;
	}
	CurrentStimulusLocation = StimulusSource;

	//Find the nearest AI to the stimulus
	AAIBaseController* NearestAI = nullptr;
	float CurrentClosestDistance = 1e10f; // big enough

	for (AAIBaseController* AI : AIUnits)
	{
		if (AI->IsDead())
		{
			continue;
		}

		const float SqrDist = (AI->GetPawn()->GetActorLocation() - StimulusSource).SizeSquared();
		
		if (SqrDist < CurrentClosestDistance)
		{
			CurrentClosestDistance = SqrDist;
			NearestAI = AI;
		}
	}

	NearestTemp = NearestAI;

	//The cell either has no AI or all the AI are dead
	if (NearestAI == nullptr)
	{
		return;
	}

	//This stimulus was not global, lets determine if we should search
	if (Strength >= 0)
	{
		NearestAI->DetermineSearch(StimulusSource, Strength);
	}

	//This stimulus was global, lets gurantee an investigation from the nearest AI
	else
	{
		NearestAI->SetState("Investigate");
	}
}

void AAICell::RegisterInvestigator(bool StartingInvestigation)
{
	//Don't care about investigators if anyone is aggressive
	if (AgressorCount > 0)
	{
		return;
	}

	const int Change = StartingInvestigation ? 1 : -1;
	InvestigationCount += Change;

	//No AI in this cell were previously investigating
	if (StartingInvestigation && InvestigationCount == 1)
	{
		//Lets let every AI be able to turn towards the source
		for (AAIBaseController* AI : AIUnits)
		{
			AI->SetStartingSearch(true);
			//TODO this just faces towards the player
			AI->SetInvestigationLocation(AI->GetTarget()->GetActorLocation());		
		}

		SetAllUnitStates("Agitated", false, NearestTemp);
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
	if (AgressorCount <= 0)
	{
		RegisterAudioStimulus(FVector::ZeroVector, 0);
		SignalCellStimulus(Player->GetActorLocation(), -1);
	}
}

void AAICell::RegisterAudioStimulus(FVector Location, float Volume)
{
	CurrentStimulusLocation = FVector::ZeroVector;

	if (Volume > 0)
	{
		UAISense_Hearing::ReportNoiseEvent(GetWorld(), Location, Volume, Player);
	}
}

void AAICell::SetAllUnitStates(const FString NewState, const bool bIgnoreLower, AAIBaseController* Ignore)
{
	for (AAIBaseController* AI : AIUnits)
	{
		if (AI->IsDead() || AI == Ignore)
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