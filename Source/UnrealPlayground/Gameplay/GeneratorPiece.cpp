#include "GeneratorPiece.h"
#include "HealthComponent.h"


// Sets default values
AGeneratorPiece::AGeneratorPiece()
{
	Health = CreateDefaultSubobject<UHealthComponent>("Health");
}

// Called when the game starts or when spawned
void AGeneratorPiece::BeginPlay()
{
	Super::BeginPlay();

	Health->OnDeath.AddDynamic(this, &AGeneratorPiece::BroadcastDisable);
}

void AGeneratorPiece::BroadcastDisable()
{
	OnGeneratorPieceHit.Broadcast();

	/* HIDE piece once hit. If we want to change this in the future 
	possibly to set a new mesh we can. If we no longer need to hide,
	make sure to remove implementation of IRemovable*/
	RemoveSelf(this);
}

