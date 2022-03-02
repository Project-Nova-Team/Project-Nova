#include "Weapon.h"
#include "CombatComponent.h"
#include "GameFramework/PlayerController.h"
#include "Components/SkeletalMeshComponent.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bStartWithTickEnabled = false;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetCollisionProfileName("Ragdoll");	
	Mesh->SetSimulatePhysics(true);

	ThrowForce = 100000.f;

	BaseDamage = 25.f;
	ImpulseKickFactor = .3f;

	ActionMappingName = TEXT("Interact");
}

void AWeapon::InteractionEvent(APawn* EventSender)
{
	//We are already being held by another combat component
	if (OwningComponent != nullptr)
	{
		return;
	}
	
	//If the EventSender has a combat component, pick this weapon up
	if (UCombatComponent* Combat = EventSender->FindComponentByClass<UCombatComponent>())
	{
		Combat->PickUpWeapon(this);
		OwningComponent = Combat;
	}
}

void AWeapon::SetCombatComponent(UCombatComponent* NewOwner)
{
	//A pawn has picked the weapon up
	if (NewOwner != nullptr)
	{
		//Set properties involving world		
		PrimaryActorTick.SetTickFunctionEnable(true);

		//Set properties involving animation
		Mesh->SetRelativeLocation(AnimData.AbsoluteLocationOffset);
		Mesh->SetRelativeRotation(AnimData.AbsoluteRotationOffset);
		TraceOrigin = NewOwner->GetTraceOrigin();

		bReportHUDEvents = NewOwner->GetUpdatesHUD();
		if (bReportHUDEvents)
		{
			NotifyHUD();
		}
	}

	//A pawn has dropped the weapon
	else
	{
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		/** Stop reporting UI events*/
		bReportHUDEvents = false;

		//Re-enable world values
		Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Mesh->SetSimulatePhysics(true);
		PrimaryActorTick.SetTickFunctionEnable(false);

		//Apply a force to make it look like the gun was thrown
		Mesh->AddForce(TraceOrigin->GetForwardVector() * ThrowForce);
		TraceOrigin = nullptr;
	}

	OwningComponent = NewOwner;
}

void AWeapon::NotifyHUD()
{
	if (bReportHUDEvents && OwningComponent != nullptr)
	{
		OwningComponent->UpdateHUD();
	}
}

void AWeapon::InteractionEvent(APawn* EventSender)
{
	//We are already being held by another combat component
	//This could happen if two pawns attempt to pick up the weapon on the same frame
	if (OwningComponent != nullptr)
	{
		return;
	}

	//If the EventSender has a combat component, pick this weapon up
	if (UCombatComponent* Combat = EventSender->FindComponentByClass<UCombatComponent>())
	{
		Combat->PickUpWeapon(this);
	}
}

void AWeapon::RecieveLookedAt(APawn* EventSender)
{
	if (CanInteract())
	{
		BindingIndex = EventSender->InputComponent->BindAction<FShooterBindingEvent>(ActionMappingName, 
			IE_Pressed, this, &AWeapon::InteractionEvent, EventSender).GetHandle();
	}
}

void AWeapon::RecieveLookedAway(APawn* EventSender, int32 MappingIndexToRemove)
{
	// Remove the delegate tied to the this object's desired ActionMapping
	EventSender->InputComponent->RemoveActionBindingForHandle(MappingIndexToRemove);
}