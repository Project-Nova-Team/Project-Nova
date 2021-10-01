#include "AIBaseController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "InstructionComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "../Gameplay/HealthComponent.h"
#include "AIBase.h"
#include "BrainComponent.h"

AAIBaseController::AAIBaseController()
{
	Perception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception"));
}

void AAIBaseController::BeginPlay()
{
	Super::BeginPlay();

	const AAIBase* OwnerAsBase = Cast<AAIBase>(GetPawn());

	if (OwnerAsBase == nullptr)
	{
		return;
	}

	Blackboard = NewObject<UBlackboardComponent>(this, TEXT("Blackboard Component"));
	if (Blackboard != nullptr)
	{
		InitializeBlackboard(*Blackboard, *BaseBoard);
		Blackboard->RegisterComponent();
	}

	//Fetch owner component references
	UInstructionComponent* InstructionComp = OwnerAsBase->GetInstruction();
	InstructionComp->Initialize(GetBlackboardComponent());
	UHealthComponent* HealthComp = OwnerAsBase->GetHealth();
	
	//Binding to delegates
	InstructionComp->OnInstructionStateChange.AddDynamic(this, &AAIBaseController::RunNewTree);
	HealthComp->OnDeath.AddDynamic(this, &AAIBaseController::OnOwnerDeath);
	Perception->OnTargetPerceptionUpdated.AddDynamic(InstructionComp, &UInstructionComponent::OnStimulus);

	RunBehaviorTree(PatrolTree);
}

void AAIBaseController::RunNewTree(EInstructionState NewState)
{
	switch (NewState)
	{
	case EInstructionState::Patrol:
		RunBehaviorTree(PatrolTree);
		break;
	case EInstructionState::Search:
		RunBehaviorTree(SearchTree);
		break;
	case EInstructionState::Attack:
		RunBehaviorTree(AttackTree);
		break;
	}
}

void AAIBaseController::OnOwnerDeath()
{
	FAISenseID HearingSenseID = UAISense::GetSenseID<
	GetBrainComponent()->Cleanup();
	Perception->UpdatePerceptionWhitelist(FAISenseID::FAINamedID("Sight"), false);
}
