#include "ShooterCombatComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "../ShooterGameMode.h"
#include "../Utility/DelayedActionManager.h"

UShooterCombatComponent::UShooterCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	/**
		we know that the swap animation takes this long. Hardcoded because we can't directly get length of anim.
		When we replace the placeholder anim, we need to change this float.
	*/
	SwapLockoutTime = .75f;
	ThrowLockoutTime = 0.8f;
	MeleeLockoutTime = 1.5f;

	NoWeaponBloom = 1.f;
	NoWeaponRecoilRecovery = 30.f;
}

void UShooterCombatComponent::SetUpConstruction(USceneComponent* TraceComponent, USkeletalMeshComponent* MeshComponent)
{
	Super::SetUpConstruction(TraceComponent, MeshComponent);
	Camera = Cast<UCameraComponent>(TraceOrigin);	
}

void UShooterCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	AShooterGameMode* GameMode = GetWorld()->GetAuthGameMode<AShooterGameMode>();
	DelayManager = GameMode->GetDelayedActionManager();

	StartingCameraFOV = Camera->FieldOfView;

	if (SecondaryWeapon != nullptr)
	{
		SecondaryWeapon->SetSceneValues(TraceOrigin, WeaponMesh, WeaponMesh->GetSocketByName("barrel"));
	}
}

void UShooterCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	UActorComponent::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	const bool bNoWeapon = PrimaryWeapon == nullptr;

	if (!bNoWeapon)
	{
		const bool bIsMoving = Input->bIsMoving;
		PrimaryWeapon->SetBloomMin(Input->Stance, bIsMoving);
	}

	HandleAimState(bNoWeapon);
	HandleSpecialActions();
	HandleStandardActions(bNoWeapon);	
}

void UShooterCombatComponent::PickUpNewWeapon(AWeapon* NewWeapon)
{
	USkeletalMesh* const NewMesh = NewWeapon->GetSkeletalMesh();
	WeaponMesh->SetSkeletalMesh(NewMesh);

	//We don't have any weapons, so just add it to the shooter
	if (PrimaryWeapon == nullptr)
	{
		PrimaryWeapon = NewWeapon;
		PrimaryWeapon->SetSceneValues(TraceOrigin, WeaponMesh, WeaponMesh->GetSocketByName("barrel"));
	}

	//We have a primary but no secondary, just switch the weapons
	else if (SecondaryWeapon == nullptr)
	{
		SecondaryWeapon = PrimaryWeapon;
		PrimaryWeapon = NewWeapon;
		PrimaryWeapon->SetSceneValues(TraceOrigin, WeaponMesh, WeaponMesh->GetSocketByName("barrel"));
	}

	//We have 2 weapons, drop our current one for the new one
	else
	{
		PrimaryWeapon->SetSceneValues(nullptr, nullptr, nullptr);
		PrimaryWeapon = NewWeapon;	
		PrimaryWeapon->SetSceneValues(TraceOrigin, WeaponMesh, WeaponMesh->GetSocketByName("barrel"));
	}
}

void UShooterCombatComponent::AddAmmmoToWeapon(AWeapon* Weapon, int AmmoAddAmount)
{
	Weapon->AddExcessAmmo(AmmoAddAmount);
}

void UShooterCombatComponent::SwapWeapons()
{
	if (PrimaryWeapon == nullptr || SecondaryWeapon == nullptr)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Swapped"));
  
	AWeapon* const Temp = PrimaryWeapon;
	PrimaryWeapon = SecondaryWeapon;
	SecondaryWeapon = Temp;

	WeaponMesh->SetSkeletalMesh(PrimaryWeapon->GetSkeletalMesh());
	PrimaryWeapon->SetSceneValues(TraceOrigin, WeaponMesh, WeaponMesh->GetSocketByName("barrel"));

	// Lockout reset immediately - this allows the user to shoot as soon as weapon is swapped.
	// If we would not like the player to be able to shoot right away, change resetlockout to resetlockoutafterdelay
}

void UShooterCombatComponent::HandleSpecialActions()
{
	//We don't perform any actions if we are aiming or in the middle of another action
	if (bIsAimed || bIsLockedOut)
	{
		return;
	}

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
}

void UShooterCombatComponent::HandleStandardActions(const bool bNoWeapon)
{
	// Makes sure swap input cannot occur while locked out
	if (bIsLockedOut)
	{
		if (Input->bIsTryingToSwap)
		{
			Input->bIsTryingToSwap = false;
		}

		return;
	}

	//We don't perform these actions without a weapon or during lockout
	if (bNoWeapon)
	{
		return;
	}

	if (Input->bIsTryingToReload)
	{
		PrimaryWeapon->Reload();
		Input->bIsTryingToReload = false;
		//We probably want to use the animation to reset lockout and actually call the reload function on the weapon
		//Lets figure out how to do that instead of using the DelayedActionManager
	}

	else if (Input->bIsTryingToSwap)
	{
		bIsLockedOut = true;
		BroadcastSwapEvent();
		//bIsLockedOut = true;
		//ResetLockoutAfterDelay(SwapLockoutTime);
		//// Swaps Weapons on delay

		////SwapWeapons();
		Input->bIsTryingToSwap = false;
	}

	else if (Input->bIsTryingToFire)
	{		
		if (!bIsLockedOut)
		{
			//Semi auto functionality
			if (PrimaryWeapon->GetWeaponType() == FT_Semi)
			{
				Input->bIsTryingToFire = false;
			}

			//USkeletalMeshComponent* Mesh = Cast<USkeletalMeshComponent>(GetOwner()->FindComponentByClass<USkeletalMeshComponent>()->GetChildComponent(0));
			//FVector ArmsLocation = Mesh->GetComponentToWorld().GetLocation();
			FRotator Rotation = Camera->GetAttachParent()->GetForwardVector().ToOrientationRotator();

			PrimaryWeapon->FireWithNoise(bIsAimed, Rotation);
		}	
	}
}

void UShooterCombatComponent::HandleAimState(const bool bNoWeapon)
{
	//Second condition only evaluates if something weird happenes (losing weapon while aiming)
	if (bIsAimed && !Input->bIsTryingToAim || (bNoWeapon && bIsAimed))
	{
		bIsAimed = false;
		Camera->FieldOfView = StartingCameraFOV; // TODO - hard coded value
		//Invoke animation event here
	}

	else if (!bNoWeapon && !bIsAimed && Input->bIsTryingToAim)
	{
		bIsAimed = true;
		Camera->FieldOfView = PrimaryWeapon->AimFOV;
		//animation event
	}
}

void UShooterCombatComponent::ResetLockout()
{
	bIsLockedOut = false;
	Handle = nullptr;
}

void UShooterCombatComponent::ResetLockoutAfterDelay(const float LockoutDuration)
{
	if (Handle != nullptr)
	{
		Handle->GetAction()->StopAction();
		Handle = nullptr;
	}

	Handle = DelayManager->StartDelayedAction(this, &UShooterCombatComponent::ResetLockout, LockoutDuration);
}

float UShooterCombatComponent::GetWeaponRecoilVelocity() const
{
	return PrimaryWeapon == nullptr ? 0.f : PrimaryWeapon->GetRecoilVelocity();
}

float UShooterCombatComponent::GetWeaponBloom() const
{
	return PrimaryWeapon == nullptr ? NoWeaponBloom : PrimaryWeapon->GetBloom();
}

float UShooterCombatComponent::GetWeaponRecoilRecovery() const
{
	return PrimaryWeapon == nullptr ? NoWeaponRecoilRecovery : PrimaryWeapon->GetRecoilRecovery();
}

void UShooterCombatComponent::BroadcastSwapEvent()
{
	// broadcast an event here that will play an anim montage in shooter blueprint!
	OnWeaponSwapRequest.Broadcast();
}