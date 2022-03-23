#include "InteractableFusebox.h"
#include "Animation/AnimInstance.h"


AInteractableFusebox::AInteractableFusebox()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);

	bCanInteract = true;
}

void AInteractableFusebox::InteractionEvent(APawn* EventSender)
{
	SetCanInteract(false);
	
	OnInteract.Broadcast(EventSender);
}

void AInteractableFusebox::SetCanInteract(bool Status)
{
	bCanInteract = Status;
}
