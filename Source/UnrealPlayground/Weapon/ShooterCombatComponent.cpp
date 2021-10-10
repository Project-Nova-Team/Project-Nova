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

void UShooterCombatComponent::PickUpNewWeapon(AGun* NewWeapon)
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
	else if(WeaponArray[1] == nullptr)
	{
		WeaponArray.RemoveAt(1);
		WeaponArray.EmplaceAt(1, OffhandGun);
	}

	if (WeaponArray[0] != nullptr)
	{
		WeaponArray.RemoveAt(0);
		WeaponArray.EmplaceAt(0, PrimaryGun);
	}

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
	if (PrimaryGun == nullptr || OffhandGun == nullptr)
	{
		return;
	}

	if (CurrentWeapon->IsA(AMeleeWeapon::StaticClass()))
	{
		CurrentWeaponIndex = 2;
	}

	//check next weapon in array
	if (SwapDirection == SS_Up)
	{
		// If we are already at third weapon and you scroll up, return index to 0
		if (CurrentWeaponIndex == 2)
			CurrentWeaponIndex = 0;
		else
			CurrentWeaponIndex++;

		if (WeaponArray[CurrentWeaponIndex]->IsA(AGun::StaticClass()))
		{
			UE_LOG(LogTemp, Warning, TEXT("Current Weapon Index: %d"), CurrentWeaponIndex);

			AGun* const Temp = PrimaryGun;
			PrimaryGun = OffhandGun;
			OffhandGun = Temp;

			WeaponMesh->SetSkeletalMesh(PrimaryGun->GetSkeletalMesh());
			PrimaryGun->SetGunSceneValues(TraceOrigin, WeaponMesh, WeaponMesh->GetSocketByName("barrel"));

			CurrentWeapon = PrimaryGun;
		}
		else
		{
			WeaponMesh->SetSkeletalMesh(MeleeWeapon->GetSkeletalMesh());
			MeleeWeapon->SetWeaponSceneValues(TraceOrigin, WeaponMesh);

			UE_LOG(LogTemp, Warning, TEXT("Current Weapon Index: %d"), CurrentWeaponIndex);

			CurrentWeapon = MeleeWeapon;
		}
	}
	else if (SwapDirection == SS_Down)
	{
		// If we are already at 1st weapon and you scroll down, return index to last
		if (CurrentWeaponIndex == 0)
			CurrentWeaponIndex = 2;
		else
			CurrentWeaponIndex--;

		if (WeaponArray[CurrentWeaponIndex]->IsA(AGun::StaticClass()))
		{
			UE_LOG(LogTemp, Warning, TEXT("Current Weapon Index: %d"), CurrentWeaponIndex);

			AGun* const Temp = PrimaryGun;
			PrimaryGun = OffhandGun;
			OffhandGun = Temp;

			WeaponMesh->SetSkeletalMesh(PrimaryGun->GetSkeletalMesh());
			PrimaryGun->SetGunSceneValues(TraceOrigin, WeaponMesh, WeaponMesh->GetSocketByName("barrel"));

			CurrentWeapon = PrimaryGun;
		}
		else 
		{
			WeaponMesh->SetSkeletalMesh(MeleeWeapon->GetSkeletalMesh());
			MeleeWeapon->SetWeaponSceneValues(TraceOrigin, WeaponMesh);

			UE_LOG(LogTemp, Warning, TEXT("Current Weapon Index: %d"), CurrentWeaponIndex);

			CurrentWeapon = MeleeWeapon;
		}
	}

	// Reset Weapons in array
	if (WeaponArray[0] != nullptr)
	{
		WeaponArray.RemoveAt(0);
		WeaponArray.EmplaceAt(0, PrimaryGun);
	}
		
	if (WeaponArray[1] != nullptr)
	{
		WeaponArray.RemoveAt(1);
		WeaponArray.EmplaceAt(1, OffhandGun);
	}
		
	if (WeaponArray[2] != nullptr)
	{
		WeaponArray.RemoveAt(2);
		WeaponArray.EmplaceAt(2, MeleeWeapon);
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

	else if (Input->bIsTryingToSwapUp)
	{
		SwapState = SS_Up;
		bIsLockedOut = true;
		BroadcastSwapEvent();
		//bIsLockedOut = true;
		//ResetLockoutAfterDelay(SwapLockoutTime);
		//// Swaps Weapons on delay

		////SwapWeapons();
		Input->bIsTryingToSwapUp = false;
	}

	else if (Input->bIsTryingToSwapDown)
	{
		SwapState = SS_Down;
		bIsLockedOut = true;
		BroadcastSwapEvent();

		Input->bIsTryingToSwapDown = false;
	}

	else if (Input->bIsTryingToFire)
	{		
		if (!bIsLockedOut)
		{
			if (CurrentWeapon->IsA(AGun::StaticClass()))
			{
				//Semi auto functionality
				if (PrimaryGun->GetWeaponType() == FT_Semi)
				{
					Input->bIsTryingToFire = false;
				}

				//USkeletalMeshComponent* Mesh = Cast<USkeletalMeshComponent>(GetOwner()->FindComponentByClass<USkeletalMeshComponent>()->GetChildComponent(0));
				//FVector ArmsLocation = Mesh->GetComponentToWorld().GetLocation();
				FRotator Rotation = Camera->GetAttachParent()->GetForwardVector().ToOrientationRotator();

				PrimaryGun->FireWithNoise(bIsAimed, Rotation);
			}
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

//void UShooterCombatComponent::SetWeaponSceneValues(AWeapon* Weapon, const USceneComponent* TraceOriginComponent, const USkeletalMeshComponent* HeldWeapon, const USkeletalMeshSocket* BulletSocket)
//{
//	//A pawn has picked the weapon up
//	if (TraceOriginComponent != nullptr)
//	{
//		//Hide actor
//		Weapon->GetMeshComponent()->SetVisibility(false);
//		Weapon->GetMeshComponent()->SetSimulatePhysics(false);
//		Weapon->GetMeshComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
//
//		Weapon->SetActorTick(true);
//	}
//
//	//A pawn has dropped the weapon
//	else
//	{
//		//Move the gun a little below the camera 
//		const FVector DropLocation = Weapon->TraceOrigin->GetComponentLocation() - FVector(0, 0, 30.f);
//		Weapon->SetActorLocation(DropLocation);
//
//		//Display actor
//		Weapon->GetMeshComponent()->SetVisibility(true);
//		Weapon->GetMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
//		Weapon->GetMeshComponent()->SetSimulatePhysics(true);
//
//		//Apply a force to make it look like the gun was thrown
//		Weapon->GetMeshComponent()->AddForce(Weapon->TraceOrigin->GetForwardVector().GetSafeNormal2D() * Weapon->ThrowForce);
//
//		Weapon->SetActorTickEnabled(false);
//	}
//
//	Weapon->TraceOrigin = TraceOriginComponent;
//	Weapon->SetHeldWeaponMesh(HeldWeapon);
//}