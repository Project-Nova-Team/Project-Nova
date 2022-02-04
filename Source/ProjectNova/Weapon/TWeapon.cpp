#include "TWeapon.h"
#include "TCombatComponent.h"
#include "GameFramework/PlayerController.h"

ATWeapon::ATWeapon()
{
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
}

void ATWeapon::SetCombatComponent(UTCombatComponent* NewOwner)
{	
	//A pawn has picked the weapon up
	if (NewOwner != nullptr)
	{
		//Set properties involving world
		Mesh->SetSimulatePhysics(false);
		Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		PrimaryActorTick.SetTickFunctionEnable(true);

		//Set properties involving animation
		if (IKData.bUseData)
		{
			Mesh->SetRelativeLocation(IKData.AbsoluteLocationOffset);
			Mesh->SetRelativeRotation(IKData.AbsoluteRotationOffset);
		}

		//We don't bother with a cast check because this function should only be called when a pawn picks us up
		APawn* Pawn = NewOwner->GetOwner<APawn>();

		//Only pawns controlled by player controllers should have a HUD
		APlayerController* Controller = Cast<APlayerController>(Pawn->GetController());

		if (Controller != nullptr && Controller->MyHUD != nullptr)
		{
			bReportHUDEvents = NewOwner->GetUpdatesHUD();
		}
	}

	//A pawn has dropped the weapon
	else
	{
		//This should be safe because a component needs an actor to be valid
		AActor* ActorOwner = OwningComponent->GetOwner();

		/** Stop reporting UI events*/
		bReportHUDEvents = false;

		//Re-enable world values
		Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Mesh->SetSimulatePhysics(true);
		PrimaryActorTick.SetTickFunctionEnable(false);
		DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);

		//Apply a force to make it look like the gun was thrown
		Mesh->AddForce(ActorOwner->GetActorForwardVector().GetSafeNormal2D() * ThrowForce);
	}

	OwningComponent = NewOwner;
}

void ATWeapon::NotifyHUD() const
{
	if (bReportHUDEvents && OwningComponent != nullptr)
	{
		OwningComponent->UpdateHUD();
	}
}

void ATWeapon::InteractionEvent(APawn* EventSender)
{
	//We are already being held by another combat component
	//This could happen if two pawns attempt to pick up the weapon on the same frame
	if (OwningComponent != nullptr)
	{
		return;
	}

	//If the EventSender has a combat component, pick this weapon up
	if (UTCombatComponent* Combat = EventSender->FindComponentByClass<UTCombatComponent>())
	{
		Combat->PickUpWeapon(this);
	}
}