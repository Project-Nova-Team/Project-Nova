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
		for (AWeapon* w : Arsenal)
		{
			w->SetGunSceneValues(TraceOrigin, WeaponMesh, WeaponMesh->GetSocketByName("barrel"));
		}
	}

	CurrentWeaponIndex = 0;
}

void UCombatComponent::PickUpNewGun(AGun* NewWeapon)
{
	USkeletalMesh* const NewMesh = NewWeapon->GetSkeletalMesh();
	WeaponMesh->SetSkeletalMesh(NewMesh);

}

void UCombatComponent::AddAmmmoToWeapon(AGun* Weapon, int AmmoAddAmount)
{
	Weapon->AddExcessAmmo(AmmoAddAmount);
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
		Arsenal[CurrentWeaponIndex]->Attack(bIsAimed);
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

float UCombatComponent::GetWeaponRecoilVelocity() const
{
	AGun* WeaponAsGun = Cast<AGun>(Arsenal[CurrentWeaponIndex]);

	//Only guns have recoil velocity
	if (WeaponAsGun != nullptr)
	{
		return WeaponAsGun->GetRecoilVelocity();
	}
	
	return 0;
}

float UCombatComponent::GetWeaponRecoilRecovery() const
{
	AGun* WeaponAsGun = Cast<AGun>(Arsenal[CurrentWeaponIndex]);

	//Only guns have recoil recovery
	if (WeaponAsGun != nullptr)
	{
		return WeaponAsGun->GetRecoilRecovery();
	}

	return NoWeaponRecoilRecovery;
}

float UCombatComponent::GetWeaponBloom() const
{
	AGun* WeaponAsGun = Cast<AGun>(Arsenal[CurrentWeaponIndex]);

	//Only guns have bloom
	if (WeaponAsGun != nullptr)
	{
		return WeaponAsGun->GetBloom();
	}

	return NoWeaponBloom;
}