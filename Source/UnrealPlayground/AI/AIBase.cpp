#include "AIBase.h"
#include "InstructionComponent.h"

AAIBase::AAIBase()
{
	PrimaryActorTick.bCanEverTick = true;

	Instruction = CreateDefaultSubobject<UInstructionComponent>(TEXT("Instruction"));
}

void AAIBase::BeginPlay()
{
	Super::BeginPlay();
}

void AAIBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}