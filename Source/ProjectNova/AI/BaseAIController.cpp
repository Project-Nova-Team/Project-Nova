#include "BaseAIController.h"
#include "BaseAI.h"
#include "../Gameplay/HealthComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

ABaseAIController::ABaseAIController()
{
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception"));

	// We are forcing every AI to have these three senses although, in some cases, particular AI may never need one
	// Its probably better to have sub classes of this base class determine the sense
	PerceptionComponent->ConfigureSense(*CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight")));
	PerceptionComponent->ConfigureSense(*CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("Hearing")));
	PerceptionComponent->ConfigureSense(*CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("Damage")));

	SetGenericTeamId(FGenericTeamId(1));
}

void ABaseAIController::SetLogicEnabled(const bool bEnableLogic)
{
	if (bEnableLogic == bLogicEnabled)
	{
		return;
	}

	bLogicEnabled = bEnableLogic;

	SetSensesEnabled(bEnableLogic);
	SetBehaviorTreeEnabled(bEnableLogic);
}

void ABaseAIController::SetSensesEnabled(const bool bEnableSenses)
{
	//First time using this type of semanatic for loops. Not sure if its proper. Rather peculiar...
	for (auto Iter = PerceptionComponent->GetSensesConfigIterator(); Iter; ++Iter)
	{
		//We don't need to check if the state is different because the implementation does it for us
		//Saves on perfomance since we don't need to update stimulus listeners
		PerceptionComponent->SetSenseEnabled((*Iter)->GetSenseImplementation(), bEnableSenses);
	}
}

void ABaseAIController::SetBehaviorTreeEnabled(const bool bEnableTree)
{
	if (!bEnableTree)
	{
		BrainComponent->StopLogic(TEXT("Loaded"));
	}

	else
	{
		if (BrainComponent != nullptr)
		{
			BrainComponent->StartLogic();
		}

		else
		{
			RunBehaviorTree(BehaviorTreeAsset);
		}
	}
}

ETeamAttitude::Type ABaseAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	if (const APawn* OtherPawn = Cast<APawn>(&Other)) {

		if (const IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(OtherPawn->GetController()))
		{
			//If they're not with us, they're against us
			return TeamAgent->GetGenericTeamId() == GetGenericTeamId() 
				? ETeamAttitude::Friendly 
				: ETeamAttitude::Hostile;
		}
	}

	return ETeamAttitude::Neutral;
}

void ABaseAIController::OwnerDeath()
{
	SetLogicEnabled(false);
}

void ABaseAIController::OwnerRevive()
{
	SetLogicEnabled(true);
}

void ABaseAIController::PerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (Actor == nullptr)
	{
		return;
	}

	const FAISenseID ID = Stimulus.Type;

	if (GetTeamAttitudeTowards(*Actor) == ETeamAttitude::Hostile)
	{
		if (ID == UAISense::GetSenseID<UAISense_Sight>())
		{
			ReceiveSpotEnemy(Actor, Stimulus);
		}

		else if (ID == UAISense::GetSenseID<UAISense_Hearing>())
		{
			ReceiveHearEnemy(Actor, Stimulus);
		}
	}
}

void ABaseAIController::OnPossess(APawn* InPawn)
{
	//Verify that we are only going to posses pawns that derive from BaseAI
	ABaseAI* AIPawn = Cast<ABaseAI>(InPawn);

	if (AIPawn != nullptr)
	{
		Super::OnPossess(InPawn);

		AIPawn->GetHealth()->OnDeath.AddDynamic(this, &ABaseAIController::OwnerDeath);
		AIPawn->GetHealth()->OnRevive.AddDynamic(this, &ABaseAIController::OwnerRevive);
		PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ABaseAIController::PerceptionUpdated);
		Handle = AIPawn->OnLoadStatusChanged.AddUObject(this, &ABaseAIController::SetLogicEnabled);
		bLogicEnabled = AIPawn->GetIsLoaded();
		
		if (bLogicEnabled)
		{
			SetSensesEnabled(true);

			//Note that this overrides bStartLogicOnPosses and starts the tree regardless of that value
			RunBehaviorTree(BehaviorTreeAsset);
		}		
	}
}

void ABaseAIController::OnUnPossess()
{
	//Verify that we are unpossessing an AIPawn.
	//This should always be the case but in the event its not we want to prevent a crash.
	//This still does result in undefined behavior since the super implementation is not being called

	if (GetAICharacter() != nullptr)
	{
		GetAICharacter()->GetHealth()->OnDeath.RemoveDynamic(this, &ABaseAIController::OwnerDeath);
		GetAICharacter()->GetHealth()->OnRevive.RemoveDynamic(this, &ABaseAIController::OwnerRevive);
		PerceptionComponent->OnTargetPerceptionUpdated.RemoveDynamic(this, &ABaseAIController::PerceptionUpdated);
		GetAICharacter()->OnLoadStatusChanged.Remove(Handle);

		SetSensesEnabled(false);

		//Super implementation handles logic cleanup for us
		Super::OnUnPossess();	
	}
}