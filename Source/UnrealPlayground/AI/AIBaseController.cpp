#include "AIBaseController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "InstructionComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "AIBase.h"

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

	//Set up instruction component
	UInstructionComponent* const InstructionComp = OwnerAsBase->GetInstruction();
	InstructionComp->Initialize();
	Perception->OnTargetPerceptionUpdated.AddDynamic(InstructionComp, &UInstructionComponent::OnStimulus);

	//Set blackboard default values
	UBlackboardComponent* const Comp = GetBlackboardComponent();
	const FName Instruction = "Instruction";
	Comp->SetValueAsObject(Instruction, InstructionComp);

	const bool bHasPath = InstructionComp->HasPatrolPath();
	const FName Path = "HasPatrolPath";
	Comp->SetValueAsBool(Path, bHasPath);
}
