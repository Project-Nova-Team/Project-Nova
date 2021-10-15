#include "CombatComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "MeleeWeapon.h"
#include "Gun.h"

UCombatComponent::UCombatComponent()
{
	NoWeaponBloom = 1.f;
	NoWeaponRecoilRecovery = 30.f;
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
			Weapon->SetWeaponSceneValues(TraceOrigin);
		}

		PickUpWeapon(Arsenal[0]);
	}
}

void UCombatComponent::PickUpWeapon(AWeapon* NewWeapon)
{
	Arsenal.Emplace(NewWeapon);
	//We are holding too many weapons, drop the currently held one
	if (Arsenal.Num() > MaxWeaponCount)
	{
		Arsenal[CurrentWeaponIndex]->SetWeaponSceneValues(nullptr);
		Arsenal.SwapMemory(CurrentWeaponIndex, Arsenal.Num() - 1);
		Arsenal.RemoveAt(Arsenal.Num() - 1, true);
	}
	
	USkeletalMesh* const NewMesh = NewWeapon->GetSkeletalMesh();
	WeaponMesh->SetSkeletalMesh(NewMesh);

	//Play swap animation when picking up a new weapon
	OnSwap.Broadcast();
}

void UCombatComponent::ReceiveSwap(const int32 Direction)
{
	const int32 WeaponCount = Arsenal.Num();
	if (WeaponCount < 2 || !IsActionLocked())
	{
		return;
	}

	const int32 LastIndex = WeaponCount - 1;
	int32 NewIndex = CurrentWeaponIndex + Direction;
	NewIndex = NewIndex > LastIndex ? 0 : NewIndex;
	NewIndex = NewIndex < 0 ? LastIndex : NewIndex;

	CurrentWeaponIndex = NewIndex;

	bIsSwapping = true;
	OnSwap.Broadcast();
}

void UCombatComponent::ReceiveAttack(const bool bAttackEnabled)
{
	if (IsActionLocked())
	{
		return;
	}
	
	if (bAttackEnabled)
	{
		Arsenal[CurrentWeaponIndex]->StartAttack();
	}
	
	else
	{
		Arsenal[CurrentWeaponIndex]->StopAttack();
	}
}

void UCombatComponent::ReceiveAim(const bool bAimEnabled)
{
	if (IsActionLocked())
	{
		return;
	}

	bIsAimed = bAimEnabled;
	bIsInAnimation = true;
	Arsenal[CurrentWeaponIndex]->SetAim(bAimEnabled);
	
	if (bAimEnabled)
	{	
		OnAimStart.Broadcast();	
	}

	else
	{
		OnAimStop.Broadcast();
	}
}


//FIX - BAD - Weapon should have each of these values

float UCombatComponent::GetWeaponRecoilVelocity() const
{	
	AGun* const WeaponAsGun = Cast<AGun>(Arsenal[CurrentWeaponIndex]);

	if (WeaponAsGun != nullptr)
	{
		WeaponAsGun->GetRecoilVelocity();
	}

	return 0;
}

float UCombatComponent::GetWeaponRecoilRecovery() const
{
	AGun* const WeaponAsGun = Cast<AGun>(Arsenal[CurrentWeaponIndex]);

	if (WeaponAsGun != nullptr)
	{
		WeaponAsGun->GetRecoilRecovery();
	}

	return NoWeaponRecoilRecovery;
}

float UCombatComponent::GetWeaponBloom() const
{
	AGun* const WeaponAsGun = Cast<AGun>(Arsenal[CurrentWeaponIndex]);

	if (WeaponAsGun != nullptr)
	{
		WeaponAsGun->GetBloom();
	}

	return NoWeaponBloom;
}