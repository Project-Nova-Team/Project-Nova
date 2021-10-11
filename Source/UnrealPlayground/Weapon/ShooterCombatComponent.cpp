#include "ShooterCombatComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "../ShooterGameMode.h"
#include "MeleeWeapon.h"
#include "Gun.h"
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
	TraceOrigin = TraceComponent;
	WeaponMesh = MeshComponent;
	Camera = Cast<UCameraComponent>(TraceOrigin);
}

void UShooterCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	AShooterGameMode* GameMode = GetWorld()->GetAuthGameMode<AShooterGameMode>();
	DelayManager = GameMode->GetDelayedActionManager();

	StartingCameraFOV = Camera->FieldOfView;

	if (OffhandGun != nullptr)
	{
		OffhandGun->SetGunSceneValues(TraceOrigin, WeaponMesh, WeaponMesh->GetSocketByName("barrel"));
	}

	// Fill Weapon slots with empty space
	WeaponArray.EmplaceAt(0, nullptr);
	WeaponArray.EmplaceAt(1, nullptr);
	WeaponArray.EmplaceAt(2, nullptr);

	CurrentWeaponIndex = 0;
}

void UShooterCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	UActorComponent::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const bool bNoWeapon = PrimaryGun == nullptr;

	if (!bNoWeapon)
	{
		const bool bIsMoving = Input->bIsMoving;
		PrimaryGun->SetBloomMin(Input->Stance, bIsMoving);
	}

	HandleAimState(bNoWeapon);
	HandleSpecialActions();
	HandleStandardActions(bNoWeapon);
}

void UShooterCombatComponent::PickUpNewGun(AGun* NewWeapon)
{
	USkeletalMesh* const NewMesh = NewWeapon->GetSkeletalMesh();
	WeaponMesh->SetSkeletalMesh(NewMesh);

	//We don't have any weapons, so just add it to the shooter
	if (PrimaryGun == nullptr)
	{
		PrimaryGun = NewWeapon;
		PrimaryGun->SetGunSceneValues(TraceOrigin, WeaponMesh, WeaponMesh->GetSocketByName("barrel"));
	}

	//We have a primary but no secondary, just switch the weapons
	else if (OffhandGun == nullptr)
	{
		OffhandGun = PrimaryGun;
		PrimaryGun = NewWeapon;
		PrimaryGun->SetGunSceneValues(TraceOrigin, WeaponMesh, WeaponMesh->GetSocketByName("barrel"));
	}

	//We have 2 weapons, drop our current one for the new one
	else
	{
		PrimaryGun->SetGunSceneValues(nullptr, nullptr, nullptr);
		PrimaryGun = NewWeapon;
		PrimaryGun->SetGunSceneValues(TraceOrigin, WeaponMesh, WeaponMesh->GetSocketByName("barrel"));
	}

	if (WeaponArray[0] == nullptr)
	{
		WeaponArray.RemoveAt(0);
		WeaponArray.EmplaceAt(0, PrimaryGun);
	}
	else if (WeaponArray[1] == nullptr)
	{
		WeaponArray.RemoveAt(1);
		WeaponArray.EmplaceAt(1, OffhandGun);
	}

	if (WeaponArray[0] != nullptr)
	{
		WeaponArray.RemoveAt(0);
		WeaponArray.EmplaceAt(0, PrimaryGun);
	}

	CurrentWeaponIndex = 0;
	CurrentWeapon = PrimaryGun;
}

void UShooterCombatComponent::PickUpMeleeWeapon(AMeleeWeapon* const NewWeapon)
{
	USkeletalMesh* const NewMesh = NewWeapon->GetSkeletalMesh();
	WeaponMesh->SetSkeletalMesh(NewMesh);

	//We don't have any weapons, so just add it to the shooter
	if (MeleeWeapon == nullptr)
	{
		MeleeWeapon = NewWeapon;
		MeleeWeapon->SetWeaponSceneValues(TraceOrigin, WeaponMesh);
	}

	CurrentWeaponIndex = 2;
	CurrentWeapon = MeleeWeapon;

	// Melee Weapon added to array when picked up. Tweak this if we add more melee weapons
	WeaponArray.RemoveAt(2);
	WeaponArray.EmplaceAt(2, MeleeWeapon);
}

void UShooterCombatComponent::AddAmmmoToWeapon(AGun* Weapon, int AmmoAddAmount)
{
	Weapon->AddExcessAmmo(AmmoAddAmount);
}

void UShooterCombatComponent::SwapWeapons(TEnumAsByte<ESwapState> SwapDirection)
{
	if (PrimaryGun == nullptr)
	{
		return;
	}

	if (SwapDirection == SS_Up)
	{
		if (GetWeaponCount() == 3)
		{
			// If we are already at 1st weapon and you scroll down, return index to last
			if (CurrentWeaponIndex == 2)
				CurrentWeaponIndex = 0;
			else
				CurrentWeaponIndex++;
		}
		else
		{
			if (CurrentWeaponIndex == 0)
			{
				CurrentWeaponIndex = 2;
			}
			else
			{
				CurrentWeaponIndex = 0;
			}
		}

		AWeapon* NextWeapon = WeaponArray[CurrentWeaponIndex];

		if (NextWeapon != nullptr)
		{
			// If next weapon is a gun, set up gun things. Otherwise set up melee things.
			if (NextWeapon->IsA(AGun::StaticClass()))
			{
				SwapToGun();
			}
			else
			{
				SwapToMelee();
			}
		}
	}
	else if (SwapDirection == SS_Down)
	{
		if (GetWeaponCount() == 3)
		{
			// If we are already at 1st weapon and you scroll down, return index to last
			if (CurrentWeaponIndex == 0)
				CurrentWeaponIndex = 2;
			else
				CurrentWeaponIndex--;
		}
		else
		{
			if (CurrentWeaponIndex == 0)
			{
				CurrentWeaponIndex = 2;
			}
			else
			{
				CurrentWeaponIndex = 0;
			}
		}

		AWeapon* NextWeapon = WeaponArray[CurrentWeaponIndex];

		if (NextWeapon != nullptr)
		{
			// If next weapon is a gun, set up gun things. Otherwise set up melee things.
			if (NextWeapon->IsA(AGun::StaticClass()))
			{
				SwapToGun();
			}
			else
			{
				SwapToMelee();
			}
		}
	}
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
		if (Input->bIsTryingToSwapUp)
		{
			Input->bIsTryingToSwapUp = false;
		}
		else if (Input->bIsTryingToSwapDown)
		{
			Input->bIsTryingToSwapDown = false;
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
		PrimaryGun->Reload();
		Input->bIsTryingToReload = false;
		//We probably want to use the animation to reset lockout and actually call the reload function on the weapon
		//Lets figure out how to do that instead of using the DelayedActionManager
	}


	else if (Input->bIsTryingToFire)
	{
		if (!bIsLockedOut)
		{
			if (CurrentWeapon->IsA(AGun::StaticClass()))
			{
				FRotator Rotation = Camera->GetAttachParent()->GetForwardVector().ToOrientationRotator();

				//Semi auto functionality
				if (PrimaryGun->GetWeaponType() == FT_Semi)
				{
					Input->bIsTryingToFire = false;
					PrimaryGun->FireWithNoise(bIsAimed, Rotation);
				}
				else if (PrimaryGun->GetWeaponType() == FT_Burst)
				{
					for (int i = 0; i < (PrimaryGun->GetClipSize()/2); i++)
						PrimaryGun->FireShotgun(bIsAimed, Rotation);
					Input->bIsTryingToFire = false;
				}
				else
				{
					PrimaryGun->FireWithNoise(bIsAimed, Rotation);
				}
			}
		}
	}

	if (GetWeaponCount() > 1)
	{
		if (Input->bIsTryingToSwapUp)
		{
			// set swap state
			SwapState = SS_Up;
			// lock user out of actions before swap is called
			bIsLockedOut = true;
			// broadcast the event to anim blueprint
			BroadcastSwapEvent();
			// stop input because we are in tick
			Input->bIsTryingToSwapUp = false;
		}
		else if (Input->bIsTryingToSwapDown)
		{
			SwapState = SS_Down;
			bIsLockedOut = true;
			BroadcastSwapEvent();
			Input->bIsTryingToSwapDown = false;
		}
	}
}

void UShooterCombatComponent::HandleAimState(const bool bNoWeapon)
{
	//Second condition only evaluates if something weird happenes (losing weapon while aiming)
	if (bIsAimed && !Input->bIsTryingToAim || (bNoWeapon && bIsAimed))
	{
		bIsAimed = false;
		Camera->FieldOfView = StartingCameraFOV;
		//Invoke animation event here
	}

	else if (!bNoWeapon && !bIsAimed && Input->bIsTryingToAim && CurrentWeapon->IsA(AGun::StaticClass()))
	{
		bIsAimed = true;
		Camera->FieldOfView = PrimaryGun->AimFOV;
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
	return PrimaryGun == nullptr ? 0.f : PrimaryGun->GetRecoilVelocity();
}

float UShooterCombatComponent::GetWeaponBloom() const
{
	return PrimaryGun == nullptr ? NoWeaponBloom : PrimaryGun->GetBloom();
}

float UShooterCombatComponent::GetWeaponRecoilRecovery() const
{
	return PrimaryGun == nullptr ? NoWeaponRecoilRecovery : PrimaryGun->GetRecoilRecovery();
}

void UShooterCombatComponent::BroadcastSwapEvent()
{
	// broadcast an event here that will play an anim montage in shooter blueprint!
	OnWeaponSwapRequest.Broadcast();
}

int UShooterCombatComponent::GetWeaponCount()
{
	int count = 0;

	for (int i = 0; i < WeaponArray.Num(); i++)
	{
		if (WeaponArray[i] != nullptr)
		{
			count++;
		}
	}
	return count;
}

void UShooterCombatComponent::SwapToGun()
{
	//Second condition only evaluates if something weird happenes (losing weapon while aiming)
	if (bIsAimed)
	{
		bIsAimed = false;
		Camera->FieldOfView = StartingCameraFOV;
	}

	if (OffhandGun != nullptr)
	{
		AGun* const Temp = PrimaryGun;
		PrimaryGun = OffhandGun;
		OffhandGun = Temp;
	}

	// Switch mesh to current array weapon
	WeaponMesh->SetSkeletalMesh(WeaponArray[CurrentWeaponIndex]->GetSkeletalMesh());

	// Weapon setup
	PrimaryGun->SetGunSceneValues(TraceOrigin, WeaponMesh, WeaponMesh->GetSocketByName("barrel"));

	// Set the weapon that we are holding to current
	CurrentWeapon = PrimaryGun;
}

void UShooterCombatComponent::SwapToMelee()
{
	//Second condition only evaluates if something weird happenes (losing weapon while aiming)
	if (bIsAimed)
	{
		bIsAimed = false;
		Camera->FieldOfView = StartingCameraFOV;
	}

	// Set mesh
	WeaponMesh->SetSkeletalMesh(MeleeWeapon->GetSkeletalMesh());
	// Weapon setup
	MeleeWeapon->SetWeaponSceneValues(TraceOrigin, WeaponMesh);

	CurrentWeapon = MeleeWeapon;
}
