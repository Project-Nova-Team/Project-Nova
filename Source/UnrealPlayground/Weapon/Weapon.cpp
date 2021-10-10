#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Bullet.h"
#include "ShooterCombatComponent.h"

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
	BodyMultiplier = 1.f;
	HeadMultiplier = 2.f;
	LimbMultiplier = 0.5f;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeapon::InteractionEvent(const APawn* EventSender)
{
}

void AWeapon::SetActorTick(bool status)
{
	PrimaryActorTick.SetTickFunctionEnable(status);
}

void AWeapon::SetHeldWeaponMesh(const USkeletalMeshComponent* MeshToSet)
{
	HeldWeaponMesh = MeshToSet;
}

void AWeapon::SetWeaponSceneValues(const USceneComponent* TraceOriginComponent, const USkeletalMeshComponent* HeldWeapon)
{
	//A pawn has picked the weapon up
	if (TraceOriginComponent != nullptr)
	{
		//Hide actor
		Mesh->SetVisibility(false);
		Mesh->SetSimulatePhysics(false);
		Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		PrimaryActorTick.SetTickFunctionEnable(true);

		TraceOrigin = TraceOriginComponent;
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
	}

	if(HeldWeapon != nullptr)
		HeldWeaponMesh = HeldWeapon;
}
