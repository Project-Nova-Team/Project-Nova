#include "InteractiveObject.h"

//note this breaks if multiple pawns can invoke these events. Should probably switch to a collection
void IInteractiveObject::ReceiveLookedAt(APawn* EventSender)
{
	FInputActionBinding Binding = FInputActionBinding(GetActionName(), IE_Pressed);
	FInputActionHandlerSignature Signature;

	//Have to bind a raw ptr because interfaces are not uobjects!
	Signature.BindRaw(this, &IInteractiveObject::InteractionEvent, EventSender);
	Binding.ActionDelegate = Signature;


	BindingIndex = EventSender->InputComponent->AddActionBinding(Binding).GetHandle();
}

void IInteractiveObject::ReceiveLookedAway(APawn* EventSender)
{
	EventSender->InputComponent->RemoveActionBindingForHandle(BindingIndex);	
}

bool IInteractiveObject::CanInteract() const
{
	return true;
}

const FName IInteractiveObject::GetActionName() const
{
	return TEXT("Interact");
}

void IInteractiveObject::InteractionEvent(APawn* EventSender) 
{
	OnInteract.Broadcast(EventSender);
}