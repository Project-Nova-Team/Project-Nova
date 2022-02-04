#include "InteractableGeneratorPiece.h"
#include "Components/StaticMeshComponent.h"

void AInteractableGeneratorPiece::RecieveLookedAt(APawn* EventSender)
{
	if (CanInteract())
	{
		for (int i = 0; i < Settings->GetActionMappings().Num(); i++)
		{
			// Find Action Mapping named Interact
			if (Settings->GetActionMappings()[i].ActionName == "Interact")
			{
				UE_LOG(LogTemp, Warning, TEXT("Interact Key: %s"), *InteractKey.GetFName().ToString());
				FInputActionKeyMapping TargetMapping = Settings->GetActionMappings()[i];
				if (TargetMapping.Key != InteractKey)
				{
					UE_LOG(LogTemp, Warning, TEXT("Key Before: %s"), *TargetMapping.Key.GetFName().ToString());
					// Remove any key bindings on current interact action
					Settings->RemoveActionMapping(TargetMapping);
					// Add custom keybinding
					Settings->AddActionMapping(FInputActionKeyMapping(TEXT("Interact"), InteractKey));
					Settings->SaveKeyMappings();
					UE_LOG(LogTemp, Warning, TEXT("Key After: %s"), *Settings->GetActionMappings()[i].Key.GetFName().ToString());
				}
			}
		}
	}
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