#include "AIBaseController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "InstructionComponent.h"
#include "AIBase.h"

void AAIBaseController::BeginPlay()
{
	Super::BeginPlay();

	const AAIBase* OwnerAsBase = Cast<AAIBase>(GetPawn());
	UInstructionComponent* const InstructionComp = OwnerAsBase->GetInstruction();
	InstructionComp->Initialize();
	UBlackboardComponent* const Comp = GetBlackboardComponent();

	const FName Instruction = "Instruction";
	Comp->SetValueAsObject(Instruction, InstructionComp);

	const bool bHasPath = InstructionComp->HasPatrolPath();
	const FName Path = "HasPatrolPath";
	Comp->SetValueAsBool(Path, bHasPath);
}