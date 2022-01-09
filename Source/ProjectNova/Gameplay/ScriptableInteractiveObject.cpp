#include "ScriptableInteractiveObject.h"

AScriptableInteractiveObject::AScriptableInteractiveObject()
{
	bCanInteract = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);
}

void AScriptableInteractiveObject::InteractionEvent(APawn* EventSender)
{
	Execute_BlueprintInteract(this, EventSender);
}