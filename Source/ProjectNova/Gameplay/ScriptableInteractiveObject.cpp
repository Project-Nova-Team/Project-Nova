#include "ScriptableInteractiveObject.h"

AScriptableInteractiveObject::AScriptableInteractiveObject()
{
	bCanInteract = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);

	ActionMappingName = "Interact";
}

void AScriptableInteractiveObject::InteractionEvent(APawn* EventSender)
{
	Execute_BlueprintInteract(this, EventSender);
}

void AScriptableInteractiveObject::RecieveLookedAt(APawn* EventSender)
{
	if (CanInteract())
	{
		BindingIndex = EventSender->InputComponent->BindAction<FShooterBindingEvent>(ActionMappingName, 
			IE_Pressed, this, &AScriptableInteractiveObject::InteractionEvent, EventSender).GetHandle();
	}
}

void AScriptableInteractiveObject::RecieveLookedAway(APawn* EventSender, int32 MappingIndexToRemove)
{
	// Remove the delegate tied to the this object's desired ActionMapping
	EventSender->InputComponent->RemoveActionBindingForHandle(MappingIndexToRemove);
}
