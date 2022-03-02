#include "InteractableGeneratorPiece.h"
#include "Components/StaticMeshComponent.h"

AInteractableGeneratorPiece::AInteractableGeneratorPiece()
{
	ActionMappingName = "Interact";
}

void AInteractableGeneratorPiece::RecieveLookedAt(APawn* EventSender)
{
	if (CanInteract())
	{
		BindingIndex = EventSender->InputComponent->BindAction<FShooterBindingEvent>(ActionMappingName,
			IE_Pressed, this, &AInteractableGeneratorPiece::InteractionEvent, EventSender).GetHandle();
	}
}

void AInteractableGeneratorPiece::RecieveLookedAway(APawn* EventSender, int32 MappingIndexToRemove)
{
	// Remove the delegate tied to the this object's desired ActionMapping
	EventSender->InputComponent->RemoveActionBindingForHandle(MappingIndexToRemove);
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