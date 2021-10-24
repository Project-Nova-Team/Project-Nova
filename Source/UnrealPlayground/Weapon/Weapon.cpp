#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "CombatComponent.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh");
	SetRootComponent(Mesh);

	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetCollisionProfileName("Ragdoll");	
	Mesh->SetSimulatePhysics(true);

	ThrowForce = 100000.f;

	BaseDamage = 25.f;
	ImpulseKickFactor = .3f;
}

void AWeapon::InteractionEvent(const APawn* EventSender)
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

void AWeapon::SetWeaponSceneValues(USceneComponent* TraceOriginComponent, USkeletalMeshComponent* HeldWeapon)
{
	//A pawn has picked the weapon up
	if (TraceOriginComponent != nullptr)
	{
		//Hide actor
		Mesh->SetVisibility(false);
		Mesh->SetSimulatePhysics(false);
		Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		PrimaryActorTick.SetTickFunctionEnable(true);
	}

	//A pawn has dropped the weapon
	else
	{
		//Move the gun a little below the camera 
		const FVector DropLocation = TraceOrigin->GetComponentLocation() - FVector(0, 0, 30.f);
		SetActorLocation(DropLocation);

		//Display actor
		Mesh->SetVisibility(true);
		Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Mesh->SetSimulatePhysics(true);

		//Apply a force to make it look like the gun was thrown
		Mesh->AddForce(TraceOrigin->GetForwardVector().GetSafeNormal2D() * ThrowForce);
		PrimaryActorTick.SetTickFunctionEnable(false);
		OwningComponent = nullptr;
	}

	TraceOrigin = TraceOriginComponent;
	ProjectileOriginMesh = HeldWeapon;
}
