#include "CombatComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Gun.h"
#include "MeleeWeapon.h"

UCombatComponent::UCombatComponent()
{
	NoWeaponBloom = 1.f;
	NoWeaponRecoilRecovery = 30.f;
	CurrentWeaponIndex = 0;
	MaxWeaponCount = 4;
}

void UCombatComponent::SetUpConstruction(USceneComponent* TraceComponent, USkeletalMeshComponent* MeshComponent)
{
	TraceOrigin = TraceComponent;
	WeaponMesh = MeshComponent;
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (Arsenal.Num() > 0)
	{
		for (AWeapon* Weapon : Arsenal)
		{
			Weapon->SetWeaponSceneValues(TraceOrigin, WeaponMesh);
			OnArsenalAddition.Broadcast(Weapon);
		}

		WeaponMesh->SetSkeletalMesh(Arsenal[0]->GetSkeletalMesh());
		bIsSwapping = true;
		OnSwap.Broadcast();
	}
}

void UCombatComponent::PickUpWeapon(AWeapon* NewWeapon)
{
	Arsenal.Emplace(NewWeapon);
	//We are holding too many weapons, drop the currently held one
	if (Arsenal.Num() > MaxWeaponCount)
	{
		OnArsenalRemoval.Broadcast(Arsenal[CurrentWeaponIndex]);
		Arsenal[CurrentWeaponIndex]->SetWeaponSceneValues(nullptr, nullptr);
		Arsenal.SwapMemory(CurrentWeaponIndex, Arsenal.Num() - 1);
		Arsenal.RemoveAt(Arsenal.Num() - 1);
		WeaponMesh->SetSkeletalMesh(Arsenal[CurrentWeaponIndex]->GetSkeletalMesh());
		SwapEvent();
	}

	//We picked up a new weapon but havent gone over our weapon limit, just swap to it
	else if(Arsenal.Num() > 1)
	{		
		Arsenal[CurrentWeaponIndex]->StopAttack();
		CurrentWeaponIndex++;
		WeaponMesh->SetSkeletalMesh(Arsenal[CurrentWeaponIndex]->GetSkeletalMesh());
		SwapEvent();
	}

	//We didn't have any weapons before, so we just play a swap animation
	else
	{
		WeaponMesh->SetSkeletalMesh(Arsenal[CurrentWeaponIndex]->GetSkeletalMesh());
		SwapEvent();
	}

	Arsenal[CurrentWeaponIndex]->SetWeaponSceneValues(TraceOrigin, WeaponMesh);
	OnArsenalAddition.Broadcast(NewWeapon);
}

void UCombatComponent::SwapEvent()
{
	//Play swapping animation
	OnSwap.Broadcast();
	bIsSwapping = true;
	
	//Update UI
	Arsenal[CurrentWeaponIndex]->OnUpdateUI.ExecuteIfBound();
}

void UCombatComponent::ReceiveSwap(const int32 Direction)
{
	const int32 WeaponCount = Arsenal.Num();
	if (WeaponCount < 2 || IsNonSwapLocked())
	{
		return;
	}

	Arsenal[CurrentWeaponIndex]->StopAttack();

	//If we swap past the last or before the first index, loop to the other end
	const int32 LastIndex = WeaponCount - 1;
	int32 NewIndex = CurrentWeaponIndex + Direction;
	NewIndex = NewIndex > LastIndex ? 0 : NewIndex;
	NewIndex = NewIndex < 0 ? LastIndex : NewIndex;

	CurrentWeaponIndex = NewIndex;

	WeaponMesh->SetSkeletalMesh(Arsenal[CurrentWeaponIndex]->GetSkeletalMesh());
	SwapEvent();
}

void UCombatComponent::ReceiveSwapComplete()
{
	bIsSwapping = false;
}

void UCombatComponent::ReceiveAttack(const bool bAttackEnabled)
{
	if (IsAttackLocked())
	{
		return;
	}

	if (!bAttackEnabled)
	{
		Arsenal[CurrentWeaponIndex]->StopAttack();
	}
	
	else
	{
		OnAttack.Broadcast();
		Arsenal[CurrentWeaponIndex]->StartAttack();
		bIsInAnimation = true;	
	}
}

void UCombatComponent::ReceiveAim()
{
	//We only want to aim guns
	if (IsActionLocked() || !GetHeldWeapon()->IsAimable())
	{
		return;
	}

	bIsAimed = !bIsAimed;
	Arsenal[CurrentWeaponIndex]->SetAim(bIsAimed);

	bIsInAnimation = true;
	
	if (bIsAimed)
	{	
		OnAimStart.Broadcast();	
	}

	else
	{
		OnAimStop.Broadcast();
	}
}

void UCombatComponent::ReceiveAnimationComplete()
{
	bIsInAnimation = false;
}

void UCombatComponent::ReceiveReload()
{
	//We only want to reload guns
	if (IsReloadLocked() || !GetHeldWeapon()->IsReloadable())
	{
		return;
	}
	
	bIsReloading = true;

	//Play reload animation
	OnReload.Broadcast();

	//Don't want to keep shooting when a reload is requested
	Arsenal[CurrentWeaponIndex]->StopAttack();
}

void UCombatComponent::ReceiveReloadComplete(const bool bInterrupted)
{
	if (!bInterrupted)
	{
		Arsenal[CurrentWeaponIndex]->Reload();
	}

	bIsReloading = false;
}

//FIX - BAD - Weapon should have each of these values

float UCombatComponent::GetWeaponRecoilVelocity() const
{	
	AGun* const WeaponAsGun = Cast<AGun>(GetHeldWeapon());

	if (WeaponAsGun != nullptr)
	{
		return WeaponAsGun->GetRecoilVelocity();
	}

	return 0;
}

float UCombatComponent::GetWeaponRecoilRecovery() const
{
	AGun* const WeaponAsGun = Cast<AGun>(GetHeldWeapon());

	if (WeaponAsGun != nullptr)
	{
		return WeaponAsGun->GetRecoilRecovery();
	}

	return NoWeaponRecoilRecovery;
}

float UCombatComponent::GetWeaponBloom() const
{
	AGun* const WeaponAsGun = Cast<AGun>(GetHeldWeapon());

	if (WeaponAsGun != nullptr)
	{
		return WeaponAsGun->GetBloom();
	}

	return NoWeaponBloom;
}

float UCombatComponent::GetWeaponRecoilLimit() const
{
	AGun* const WeaponAsGun = Cast<AGun>(GetHeldWeapon());

	if (WeaponAsGun != nullptr)
	{
		return WeaponAsGun->GetRecoilLimit();
	}

	return 0;
}