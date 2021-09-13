#include "AIBaseController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "InstructionComponent.h"
#include "AIBase.h"

void AAIBaseController::BeginPlay()
{
	Super::BeginPlay();

	const FName Instruction = "Instruction";
	const AAIBase* OwnerAsBase = Cast<AAIBase>(GetPawn());

	UBlackboardComponent* Comp = GetBlackboardComponent();
	UInstructionComponent* InstructionComp = OwnerAsBase->GetInstruction();

	Comp->SetValueAsObject(Instruction, InstructionComp);
}