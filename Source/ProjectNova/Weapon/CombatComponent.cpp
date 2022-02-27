#include "CombatComponent.h"

FName UCombatComponent::WeaponSocketName = TEXT("Weapon_Socket");
FName UCombatComponent::SecondarySocketName = TEXT("Secondary_Socket");

UCombatComponent::UCombatComponent()
	: MaxWeaponCount(4)
	, bUpdatesHUD(true)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UCombatComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsTryingToAttack && !IsActionLocked())
	{
		AWeapon* Weapon = GetHeldWeapon();

		if (Weapon)
		{
			Weapon->Attack();

			if (!Weapon->bAttackContinuously)
			{
				bIsTryingToAttack = false;
			}
		}
	}
}

void UCombatComponent::SetUpConstruction(USkeletalMeshComponent* SocketMesh, USceneComponent* TraceComponent, const bool* bRunningFlag)
{
	AttachmentMesh = SocketMesh;
	TraceOrigin = TraceComponent;
	bIsRunning = bRunningFlag;
}


AWeapon* UCombatComponent::GetWeaponOfType(TSubclassOf<AWeapon> WeaponClass)
{
	for (AWeapon* Weapon : Arsenal)
	{
		if (Weapon->GetClass() == WeaponClass.Get())
		{
			return Weapon;
		}
	}

	return nullptr;
}

void UCombatComponent::PickUpWeapon(AWeapon* NewWeapon)
{
	if (NewWeapon == nullptr)
	{
		return;
	}

	//Perform any necessary set up for the weapon
	Arsenal.Emplace(NewWeapon);

	NewWeapon->GetMesh()->SetSimulatePhysics(false);
	NewWeapon->GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	NewWeapon->AttachToComponent(AttachmentMesh, FAttachmentTransformRules::KeepRelativeTransform, WeaponSocketName);

	NewWeapon->SetCombatComponent(this);
	OnWeaponAdd.ExecuteIfBound(NewWeapon);

	//We are holding too many weapons, drop the currently held one
	if (Arsenal.Num() > MaxWeaponCount)
	{
		bIsTryingToAttack = false;
		Arsenal[CurrentWeaponIndex]->SetCombatComponent(nullptr);
		Arsenal.SwapMemory(CurrentWeaponIndex, Arsenal.Num() - 1);
		Arsenal.RemoveAt(Arsenal.Num() - 1);
	}

	//We picked up a new weapon but havent gone over our weapon limit, just swap to it
	else if (Arsenal.Num() > 1)
	{
		bIsTryingToAttack = false;

		Arsenal[CurrentWeaponIndex]->GetMesh()->SetVisibility(false);

		CurrentWeaponIndex = Arsenal.Num() - 1;
	}

	//We have no weapons, just mark the index
	else
	{
		CurrentWeaponIndex = 0;
	}

	UpdateHUD();
	NotifyWeaponChange();
}

void UCombatComponent::SwapWeapon(const int32 Direction)
{
	const int32 WeaponCount = Arsenal.Num();

	if (WeaponCount < 2 || IsNonSwapLocked())
	{
		return;
	}

	bIsTryingToAttack = false;
	Arsenal[CurrentWeaponIndex]->GetMesh()->SetVisibility(false);

	//If we swap past the last or before the first index, loop to the other end
	const int32 LastIndex = WeaponCount - 1;
	int32 NewIndex = CurrentWeaponIndex + Direction;
	NewIndex = NewIndex > LastIndex ? 0 : NewIndex;
	NewIndex = NewIndex < 0 ? LastIndex : NewIndex;

	CurrentWeaponIndex = NewIndex;

	Arsenal[CurrentWeaponIndex]->GetMesh()->SetVisibility(true);

	UpdateHUD();
	NotifyWeaponChange();
}

AWeapon* UCombatComponent::GetHeldWeapon() const
{
	return Arsenal.Num() < 1 ? nullptr : Arsenal[CurrentWeaponIndex];
}

void UCombatComponent::NotifyWeaponChange()
{
	bIsSwapping = true;
	OnSwap.ExecuteIfBound();
	OnLocomotionChange.Broadcast(GetHeldWeapon() ? GetHeldWeapon() : nullptr);
}

void UCombatComponent::ReceiveReload()
{
	if (!IsReloadLocked() && GetHeldWeapon()->IsReloadable())
	{
		//Don't want to keep attacking when a reload is requested
		bIsTryingToAttack = false;

		bIsReloading = true;

		OnReload.ExecuteIfBound();
	}
}

void UCombatComponent::ReceiveStartAttack()
{
	bIsTryingToAttack = true;
}

void UCombatComponent::ReceiveStopAttack()
{
	bIsTryingToAttack = false;
}

void UCombatComponent::ReceiveAimStart()
{
	AWeapon* Weapon = GetHeldWeapon();

	if (Weapon && Weapon->IsAimable() && IsActionLocked())
	{
		bIsAimed = true;
		bIsInAnimation = true;

		OnAimStart.ExecuteIfBound();
	}
}

void UCombatComponent::ReceiveAimStop()
{
	AWeapon* Weapon = GetHeldWeapon();

	if (Weapon && Weapon->IsAimable())
	{
		bIsAimed = false;
		bIsInAnimation = true;

		OnAimStop.ExecuteIfBound();
	}
}

void UCombatComponent::ReceiveReloadComplete(const bool bInterrupted)
{
	if (!bInterrupted)
	{
		Arsenal[CurrentWeaponIndex]->Reload();
	}

	bIsReloading = false;
}

void UCombatComponent::ReceiveAnimationComplete()
{
	bIsInAnimation = false;
}

void UCombatComponent::ReceiveSwapComplete()
{
	bIsSwapping = false;
}

void UCombatComponent::UpdateHUD()
{
	OnUpdateHUD.ExecuteIfBound(GetHeldWeapon()->GetImmutableHUD());
}