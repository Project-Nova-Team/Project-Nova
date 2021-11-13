#include "Generator.h"
#include "GeneratorPiece.h"
#include "Components/StaticMeshComponent.h"

AGenerator::AGenerator()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);
}

void AGenerator::BeginPlay()
{
	Super::BeginPlay();

	for (int i = 0; i < BrokenPieces.Num(); i++)
	{
		BrokenPieces[i]->OnGeneratorPieceHit.AddUObject(this, &AGenerator::IterateHitCount);
	}
}

void AGenerator::InteractionEvent(APawn* EventSender)
{
	if (bCanInteract)
	{
		OnGeneratorInteracted.Broadcast();// Only necessary if we create a use for this

		IInteractiveObject::Execute_BlueprintInteract(this,EventSender);
	}
}

void AGenerator::IterateHitCount()
{
	HitCount++;
	CheckIfFixed();
}

void AGenerator::CheckIfFixed()
{
	if (HitCount == 3)
	{
		bCanInteract = true;
	}
}
