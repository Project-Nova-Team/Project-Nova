#include "CombatComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "../ShooterGameMode.h"
#include "../Utility/DelayedActionManager.h"
#include "AIController.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SwapLockoutTime = 0.8f;
	ThrowLockoutTime = 0.8f;
	MeleeLockoutTime = 1.5f;

	NoWeaponRecoilFallOff = 2000.f;
	NoWeaponBloomFallOff = 100.f;
	NoWeaponBloom = 1.f;
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	AShooterGameMode* GameMode = GetWorld()->GetAuthGameMode<AShooterGameMode>();
	DelayManager = GameMode->GetDelayedActionManager();

	//Determine if we are owned by an AI
	APawn* OwnerAsPawn = Cast<APawn>(GetOwner());
	AController* Controller = OwnerAsPawn->GetController();
	bOwnerIsAI = Controller->IsA<AAIController>();

	if (PrimaryWeapon != nullptr)
	{
		PrimaryWeapon->SetOwningComponent(this, TraceOrigin);
		USkeletalMesh* const NewMesh = PrimaryWeapon->GetSkeletalMesh();
		WeaponMesh->SetSkeletalMesh(NewMesh);
	}

	if (SecondaryWeapon != nullptr)
	{
		SecondaryWeapon->SetOwningComponent(this, TraceOrigin);
	}
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//We need to reset recoil/bloom if the weapon is lost so we hard code a fall off value
	const bool bNoWeapon = PrimaryWeapon == nullptr;

	if (!bOwnerIsAI)
	{
		float RecoilFallOff;
		float BloomFallOff;
		float MinBloom;

		if (bNoWeapon)
		{
			RecoilFallOff = NoWeaponRecoilFallOff;
			BloomFallOff = NoWeaponBloomFallOff;
			MinBloom = NoWeaponBloom;
		}

		else
		{
			RecoilFallOff = PrimaryWeapon->RecoilFallOff;
			BloomFallOff = PrimaryWeapon->BloomFallOff;
			
			switch (Input->Stance)
			{
			case WFS_Standing:
				MinBloom = PrimaryWeapon->BloomWalkBase;
				break;
			case WFS_Crouching:
				MinBloom = PrimaryWeapon->BloomCrouchBase;
				break;
			case WFS_Proning:
				MinBloom = PrimaryWeapon->BloomProneBase;
				break;
			}
		}

		//careful about not having a weapon here
		const float BloomMultiplier = (Input->bIsMoving && !bNoWeapon) ? GetPrimaryWeapon()->BloomBaseMovementMultiplier : 1.f;
		MinBloom *= BloomMultiplier;

		RecoilVelocity -= DeltaTime * RecoilFallOff;
		RecoilVelocity = RecoilVelocity < 0.f ? 0 : RecoilVelocity;
		CurrentBloom -= DeltaTime * BloomFallOff;
		CurrentBloom = CurrentBloom < MinBloom ? MinBloom : CurrentBloom;
	}

	//We are locked out of performing actions, break early
	if (bIsLockedOut)
	{
		return;
	}

	if (Input->bIsTryingToMelee)
	{
		//TODO melee attack implementation
		Input->bIsTryingToMelee = false;
		DelayManager->StartDelayedAction(this, &UCombatComponent::ResetLockout, MeleeLockoutTime);
	}

	else if (Input->bIsTryingToThrowPrimary)
	{
		//TODO primary throw implementation
		Input->bIsTryingToThrowPrimary = false;
		DelayManager->StartDelayedAction(this, &UCombatComponent::ResetLockout, ThrowLockoutTime);
	}

	else if (Input->bIsTryingToThrowSecondary)
	{
		//TODO secondary throw implementation
		Input->bIsTryingToThrowSecondary = false;
		DelayManager->StartDelayedAction(this, &UCombatComponent::ResetLockout, ThrowLockoutTime);
	}

	//Don't bother with the rest of the tick function if theres no weapon or if a previous action locks us out
	if (bNoWeapon || bIsLockedOut)
	{
		return;
	}

	else if (Input->bIsTryingToReload)
	{
		Input->bIsTryingToReload = false;
		//We probably want to use the animation to reset lockout and actually call the reload function on the weapon
		//Lets figure out how to do that instead of using the DelayedActionManager
	}

	else if (Input->bIsTryingToSwap)
	{
		SwapWeapons();
		Input->bIsTryingToSwap = false;
		DelayManager->StartDelayedAction(this, &UCombatComponent::ResetLockout, SwapLockoutTime);
	}

	else if (Input->bIsTryingToFire)
	{
		PrimaryWeapon->Fire();
	}


	//Check if aim state needs to change
	if (!bIsAimed && Input->bIsTryingToAim)
	{
		bIsAimed = true;
	}

	else if (bIsAimed && !Input->bIsTryingToFire)
	{
		bIsAimed = false;
	}
}

void UCombatComponent::SetUpConstruction(USceneComponent* TraceComponent, USkeletalMeshComponent* MeshComponent)
{
	TraceOrigin = TraceComponent;
	WeaponMesh = MeshComponent;
}

void UCombatComponent::PickUpNewWeapon(AWeapon* const NewWeapon)
{
	//We don't have any weapons, so just add it to the shooter
	if (PrimaryWeapon == nullptr)
	{
		PrimaryWeapon = NewWeapon;
		PrimaryWeapon->SetOwningComponent(this, TraceOrigin);
	}

	//We have a primary but no secondary, just switch the weapons
	else if (SecondaryWeapon == nullptr)
	{
		SecondaryWeapon = PrimaryWeapon;
		PrimaryWeapon = NewWeapon;
		PrimaryWeapon->SetOwningComponent(this, TraceOrigin);
	}

	//We have 2 weapons, drop our current one for the new one
	else
	{
		PrimaryWeapon->SetOwningComponent(nullptr, nullptr);
		PrimaryWeapon = NewWeapon;
		PrimaryWeapon->SetOwningComponent(this, TraceOrigin);
	}

	USkeletalMesh* const NewMesh = PrimaryWeapon == nullptr ? nullptr : PrimaryWeapon->GetSkeletalMesh();
	WeaponMesh->SetSkeletalMesh(NewMesh);
}

void UCombatComponent::SwapWeapons()
{
	if (PrimaryWeapon == nullptr || SecondaryWeapon == nullptr)
	{
		return;
	}

	AWeapon* const Temp = PrimaryWeapon;
	PrimaryWeapon = SecondaryWeapon;
	SecondaryWeapon = Temp;

	WeaponMesh->SetSkeletalMesh(PrimaryWeapon->GetSkeletalMesh());
}

void UCombatComponent::AddRecoilVelocity(const float Velocity)
{
	RecoilVelocity += Velocity;
	RecoilVelocity = RecoilVelocity > PrimaryWeapon->Recoil ? PrimaryWeapon->Recoil : RecoilVelocity;
}

void UCombatComponent::AddBloom(const float Bloom)
{
	CurrentBloom += Bloom;
	CurrentBloom = CurrentBloom > PrimaryWeapon->BloomMax ? PrimaryWeapon->BloomMax : CurrentBloom;
}