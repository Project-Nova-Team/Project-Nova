#include "CombatComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "AIController.h"
#include "Camera/CameraComponent.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (PrimaryWeapon != nullptr)
	{
		USkeletalMesh* const NewMesh = PrimaryWeapon->GetSkeletalMesh();
		WeaponMesh->SetSkeletalMesh(NewMesh);
		PrimaryWeapon->SetSceneValues(TraceOrigin, WeaponMesh, WeaponMesh->GetSocketByName("barrel"));		
	}	
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//We need to reset recoil/bloom if the weapon is lost so we hard code a fall off value
	const bool bNoWeapon = PrimaryWeapon == nullptr;

	if (Input->bIsTryingToMelee)
	{
		//TODO melee attack implementation
		Input->bIsTryingToMelee = false;
	}

	else if (Input->bIsTryingToThrowPrimary)
	{
		//TODO primary throw implementation
		Input->bIsTryingToThrowPrimary = false;
	}

	else if (Input->bIsTryingToThrowSecondary)
	{
		//TODO secondary throw implementation
		Input->bIsTryingToThrowSecondary = false;
	}

	//Don't bother with the rest of the tick function if theres no weapon or if a previous action locks us out
	if (bNoWeapon)
	{
		return;
	}

	else if (Input->bIsTryingToReload)
	{
		Input->bIsTryingToReload = false;
		//We probably want to use the animation to reset lockout and actually call the reload function on the weapon
		//Lets figure out how to do that instead of using the DelayedActionManager
	}

	else if (Input->bIsTryingToFire)
	{
		//Make code for burst fire if we really care about that
		if (PrimaryWeapon->GetWeaponType() != FT_Semi)
		{
			Input->bIsTryingToFire = false;
		}
			
		PrimaryWeapon->FireStraight();
	}
}

void UCombatComponent::SetUpConstruction(USceneComponent* TraceComponent, USkeletalMeshComponent* MeshComponent)
{
	TraceOrigin = TraceComponent;
	WeaponMesh = MeshComponent;
}

void UCombatComponent::PickUpNewWeapon(AWeapon* const NewWeapon)
{
	//We don't have any weapons, so just add it to the component
	if (PrimaryWeapon == nullptr)
	{
		PrimaryWeapon = NewWeapon;
		PrimaryWeapon->SetSceneValues(TraceOrigin, WeaponMesh, WeaponMesh->GetSocketByName("barrel"));
	}

	//We have a weapon, just swap them out
	else
	{
		PrimaryWeapon->SetSceneValues(nullptr, nullptr, nullptr);
		PrimaryWeapon = NewWeapon;
		PrimaryWeapon->SetSceneValues(TraceOrigin, WeaponMesh, WeaponMesh->GetSocketByName("barrel"));
	}

	USkeletalMesh* const NewMesh = PrimaryWeapon == nullptr ? nullptr : PrimaryWeapon->GetSkeletalMesh();
	WeaponMesh->SetSkeletalMesh(NewMesh);
}