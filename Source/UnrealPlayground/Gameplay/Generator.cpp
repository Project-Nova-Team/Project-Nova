#include "Generator.h"
#include "Components/StaticMeshComponent.h"

AGenerator::AGenerator()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);
}


void AGenerator::InteractionEvent(APawn* EventSender)
{
	//invoke event here
}