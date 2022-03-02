#include "Generator.h"
#include "GeneratorPiece.h"
#include "Components/StaticMeshComponent.h"

AGenerator::AGenerator()
{
	PrimaryActorTick.bCanEverTick = false;
	Pieces.Reserve(3);
	ScreenTextures.Reserve(5);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);

	ActionMappingName = "Interact";
}

void AGenerator::RecieveLookedAt(APawn* EventSender)
{
	if (CanInteract())
	{
		BindingIndex = EventSender->InputComponent->BindAction<FShooterBindingEvent>(ActionMappingName,
			IE_Pressed, this, &AGenerator::InteractionEvent, EventSender).GetHandle();
	}
}

void AGenerator::RecieveLookedAway(APawn* EventSender, int32 MappingIndexToRemove)
{
	// Remove the delegate tied to the this object's desired ActionMapping
	EventSender->InputComponent->RemoveActionBindingForHandle(MappingIndexToRemove);
}

void AGenerator::BeginPlay()
{
	Super::BeginPlay();

	for (AGeneratorPiece* Piece : Pieces)
	{
		Piece->OnGeneratorPieceRepair.AddDynamic(this, &AGenerator::ReceivePieceRepaired);
	}
}

void AGenerator::InteractionEvent(APawn* EventSender)
{
	bHasBeenActivated = true;
	IInteractiveObject::Execute_BlueprintInteract(this,EventSender);
}

void AGenerator::ReceivePieceRepaired()
{
	RepairedPieces++;
	OnPieceRepaired.Broadcast(RepairedPieces);
}

void AGenerator::BreakGenerator()
{
	for (AGeneratorPiece* Piece : Pieces)
	{
		Piece->BreakPiece();
	}

	RepairedPieces = 0;
	OnPieceRepaired.Broadcast(RepairedPieces);
	bHasBeenActivated = false;
}
