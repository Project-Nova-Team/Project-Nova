#include "ShooterCombatComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "../ShooterGameMode.h"
#include "../Utility/DelayedActionManager.h"

UShooterCombatComponent::UShooterCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SwapLockoutTime = 1.4f;
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

	// This is New v
	StartingCameraFOV = Camera->FieldOfView;

	if (SecondaryWeapon != nullptr)
	{
		SecondaryWeapon->SetTraceOrigin(TraceOrigin);
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

void UShooterCombatComponent::PickUpNewWeapon(AWeapon* const NewWeapon)
{
	//We don't have any weapons, so just add it to the shooter
	if (PrimaryWeapon == nullptr)
	{
		PrimaryWeapon = NewWeapon;
		PrimaryWeapon->SetTraceOrigin(TraceOrigin);
	}

	//We have a primary but no secondary, just switch the weapons
	else if (SecondaryWeapon == nullptr)
	{
		SecondaryWeapon = PrimaryWeapon;
		PrimaryWeapon = NewWeapon;
		PrimaryWeapon->SetTraceOrigin(TraceOrigin);
	}

	//We have 2 weapons, drop our current one for the new one
	else
	{
		PrimaryWeapon->SetTraceOrigin(nullptr);
		PrimaryWeapon = NewWeapon;
		PrimaryWeapon->SetTraceOrigin(TraceOrigin);
	}

	USkeletalMesh* const NewMesh = PrimaryWeapon == nullptr ? nullptr : PrimaryWeapon->GetSkeletalMesh();
	WeaponMesh->SetSkeletalMesh(NewMesh);
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

	AWeapon* const Temp = PrimaryWeapon;
	PrimaryWeapon = SecondaryWeapon;
	SecondaryWeapon = Temp;

	WeaponMesh->SetSkeletalMesh(PrimaryWeapon->GetSkeletalMesh());

	bIsLockedOut = true;
	ResetLockoutAfterDelay(SwapLockoutTime);
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
<<<<<<< Updated upstream
	// Makes sure swap input cannot occur while locked out
	if (bIsLockedOut)
	{
		if (Input->bIsTryingToSwap)
		{
			Input->bIsTryingToSwap = false;
=======
	if (bIsLockedOut)
	{
		if (bIsLockedOut)
		{
			if (Input->bIsTryingToSwap)
			{
				Input->bIsTryingToSwap = false;
			}
>>>>>>> Stashed changes
		}

		return;
	}
<<<<<<< Updated upstream

=======
	
	
>>>>>>> Stashed changes
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
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, "SwapFunc");
		SwapWeapons();	
		Input->bIsTryingToSwap = false;
	}

	else if (Input->bIsTryingToFire)
	{		
		//Make code for burst fire if we really care about that

		//Semi auto functionality
		if (PrimaryWeapon->GetWeaponType() == FT_Semi)
		{			
			Input->bIsTryingToFire = false;
		}

		PrimaryWeapon->FireWithNoise(bIsAimed);
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