#include "InteractableGeneratorPiece.h"
#include "Components/StaticMeshComponent.h"

void AInteractableGeneratorPiece::RecieveLookedAt(APawn* EventSender)
{
}

void AInteractableGeneratorPiece::BeginPlay()
{
	//We don't call super because we don't want to bind the hit delegate, we just want to have the interaction event fix the piece instead
	AActor::BeginPlay();
	BrokenMesh = Mesh->GetStaticMesh();
}

void AInteractableGeneratorPiece::InteractionEvent(APawn* EventSender)
{
	//repair the piece
	bIsRepaired = true;
	OnGeneratorPieceRepair.Broadcast();
	Mesh->SetStaticMesh(FixedMesh);

	Execute_BlueprintInteract(this, EventSender);
}