#include "ShooterAnimInstance.h"
#include "Animation/AnimMontage.h"
#include "../State/FPS/Event/SVaultState.h"
#include "../State/FPS/ShooterStateMachine.h"
#include "../Player/ShooterMovementComponent.h"
#include "../Weapon/CombatComponent.h"
#include "../Player/Shooter.h"
#include "../Weapon/MeleeWeapon.h"
#include "Camera/CameraComponent.h"


void UShooterAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	// Get Owning pawn
	Shooter = Cast<AShooter>(TryGetPawnOwner());

	if (Shooter == nullptr)
	{
		// if cast fails, try get owning actor
		Shooter = Cast<AShooter>(GetOwningActor());
	}

	ShooterMovement = Shooter->GetShooterMovement();
	ShooterMesh = Shooter->GetSkeletalMeshComponent();
	ShooterCombat = Shooter->GetCombat();
	ShooterCamera = Shooter->GetCamera();
	ShooterMelee = Shooter->GetMelee();

	ShooterCombat->OnAimStart.AddDynamic(this, &UShooterAnimInstance::PlayAimStartMontage);
	ShooterCombat->OnAimStop.AddDynamic(this, &UShooterAnimInstance::PlayAimStopMontage);
	ShooterCombat->OnReload.AddDynamic(this, &UShooterAnimInstance::PlayReloadMontage);
	ShooterCombat->OnSwap.AddDynamic(this, &UShooterAnimInstance::PlaySwapMontage);
	ShooterCombat->OnAttack.AddDynamic(this, &UShooterAnimInstance::StopMontageFromAttack);
	ShooterCombat->OnArsenalAddition.AddUObject(this, &UShooterAnimInstance::ReceiveNewWeaponPickup);
	ShooterCombat->OnArsenalRemoval.AddUObject(this, &UShooterAnimInstance::ReceiveNewWeaponDrop);

	OnMontageEnded.AddDynamic(this, &UShooterAnimInstance::MontageEnd);
}

void UShooterAnimInstance::BindVault()
{
	//This feels hacky, but might be the proper way to do it..
	//Sound/Animation have this problem of needing to be everywhere 
	UState* Raw = Shooter->GetStateMachine()->GetStateAtKey("Vaulting");
	Vault = Cast<USVaultState>(Raw);
	Vault->OnVaultEnter.AddDynamic(this, &UShooterAnimInstance::PlayVaultMontage);
}

void UShooterAnimInstance::ReceiveNewWeaponPickup(AWeapon* NewWeapon)
{
	NewWeapon->OnWeaponAttack.AddDynamic(this, &UShooterAnimInstance::PlayAttackMontage);
}

void UShooterAnimInstance::ReceiveNewWeaponDrop(AWeapon* NewWeapon)
{
	NewWeapon->OnWeaponAttack.RemoveDynamic(this, &UShooterAnimInstance::PlayAttackMontage);
}

bool UShooterAnimInstance::IsWalking()
{
	return ShooterMovement->bIsOnGround && !ShooterMovement->InputVelocity.IsNearlyZero();
}

bool UShooterAnimInstance::IsFalling()
{
	return !ShooterMovement->bIsOnGround;
}

void UShooterAnimInstance::PlayVaultMontage()
{
	Montage_Play(VaultAnimMontage, 1.0f);
}

void UShooterAnimInstance::PlaySwapMontage()
{
	Montage_Play(SwapAnimMontage, 1.0f);
}

void UShooterAnimInstance::PlayAimStartMontage()
{
	Montage_Play(AimStartAnimMontage, 1.0f);
	OnAimStart.Broadcast();
}

void UShooterAnimInstance::PlayAimStopMontage()
{
	Montage_Play(AimStopAnimMontage, 1.0f);
	OnAimStop.Broadcast();
}

void UShooterAnimInstance::PlayReloadMontage()
{
	Montage_Play(ReloadAnimMontage, 1.0f);
}

void UShooterAnimInstance::StopMontageFromAttack()
{
	StopAllMontages(0);
}

void UShooterAnimInstance::PlayAttackMontage()
{
	Montage_Play(MeleeAttackMontage, 1.0f);
}

void UShooterAnimInstance::MontageEnd(UAnimMontage* Montage, bool bInterupted)
{
	if (Montage == VaultAnimMontage)
	{
		Vault->ReceiveVaultAnimComplete();
	}

	else if (Montage == ReloadAnimMontage)
	{
		ShooterCombat->ReceiveReloadComplete(bInterupted);
	}

	else if (Montage == SwapAnimMontage)
	{
		if (bInterupted && !Montage_IsActive(SwapAnimMontage))
		{
			ShooterCombat->ReceiveSwapComplete();
		}

		else if (!bInterupted)
		{
			ShooterCombat->ReceiveSwapComplete();
		}
	}

	else if (Montage == AimStartAnimMontage || Montage == AimStopAnimMontage || Montage == MeleeAttackMontage)
	{
		ShooterCombat->ReceiveAnimationComplete();
	}
}

FTransform UShooterAnimInstance::GetWeaponSocketTransform(FName SocketName)
{
	return Shooter->GetWeaponMesh()->GetSocketTransform(SocketName);
}

void UShooterAnimInstance::StartFOVLerp(const float TargetFOV, const float Time)
{
	AimHandle = GetWorld()->
		GetAuthGameMode<AShooterGameMode>()->
		GetDelayedActionManager()->
		StartOverTimeAction(this, &UShooterAnimInstance::LerpFOV, Time, ShooterCamera->FieldOfView, TargetFOV);
}

void UShooterAnimInstance::LerpFOV(const float StartFOV, const float TargetFOV)
{
	ShooterCamera->FieldOfView = FMath::Lerp(StartFOV, TargetFOV, AimHandle->CurrentActionProgress);
}