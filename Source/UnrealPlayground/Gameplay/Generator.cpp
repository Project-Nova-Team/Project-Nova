#include "Generator.h"
#include "GeneratorPiece.h"
#include "Components/StaticMeshComponent.h"

AGenerator::AGenerator()
{
	PrimaryActorTick.bCanEverTick = false;

	bIsInteractable = false; 

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);
}

void AGenerator::BeginPlay()
{
	Super::BeginPlay();

	if (BrokenPieces.Num() > 0)
	{
		for (int i = 0; i < BrokenPieces.Num(); i++)
		{
			BrokenPieces[i]->OnGeneratorPieceHit.AddUObject(this, &AGenerator::IterateHitCount);
		}
	}
}

void AGenerator::InteractionEvent(APawn* EventSender)
{
	if (bIsInteractable)
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
		bIsInteractable = true;
	}
}
