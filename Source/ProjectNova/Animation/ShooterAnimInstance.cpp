#include "ShooterAnimInstance.h"
#include "Components/SceneComponent.h"
#include "../Player/Shooter.h"
#include "../State/FPS/ShooterStateMachine.h"
#include "../Weapon/Gun.h"

#include "DrawDebugHelpers.h"

UShooterAnimInstance::UShooterAnimInstance()
{
	LookSwayMultiplier = 10.f;
	MoveSwayMultiplier = 10.f;
	SwaySpeed = 5.f;
	AlphaHeadRotation = 0.f;

#if WITH_EDITORONLY_DATA
	bLiveUpdates = true;
#endif 
}

void UShooterAnimInstance::ReceiveWeaponSwitch(const AWeapon* NewWeapon)
{
	HeldWeapon = NewWeapon;

	if (NewWeapon)
	{
		AlphaIK = 1.f;

		NewWeapon->GetMesh()->SetRelativeLocation(NewWeapon->AnimData.AbsoluteLocationOffset);
		NewWeapon->GetMesh()->SetRelativeRotation(NewWeapon->AnimData.AbsoluteRotationOffset);

		REffectorLocation = NewWeapon->AnimData.REffectorLocationOffset;
		RTargetLocation = NewWeapon->AnimData.RTargetLocationOffset;
		LTargetLocation = NewWeapon->AnimData.LTargetLocationOffset;
	}

	//No weapon, no IK
	else
	{
		AlphaIK = 0.f;
	}

	bWeaponLocomotion = NewWeapon != nullptr;
}

void UShooterAnimInstance::NativeInitializeAnimation()
{
	//Cache this so we don't have to grab/cast it every update
	//Note this fails in anim bp preview so we will have perform an if each update
	//Perhaps consider using a preprocessor to cut out the check, although 
	//the perfomance draw of this check is negligble
	Shooter = Cast<AShooter>(GetOwningActor());

	if (Shooter)
	{
		LocomotionHandle = Shooter->GetCombat()->OnLocomotionChange.AddUObject(this, &UShooterAnimInstance::ReceiveWeaponSwitch);

		Shooter->GetCombat()->OnAimStart.BindUObject(this, &UShooterAnimInstance::ReceiveAimStart);
		Shooter->GetCombat()->OnAimStop.BindUObject(this, &UShooterAnimInstance::ReceiveAimStop);
		Shooter->GetCombat()->OnReload.BindUObject(this, &UShooterAnimInstance::ReceiveReload);
		Shooter->GetCombat()->OnSwap.BindUObject(this, &UShooterAnimInstance::ReceiveSwap);
		Shooter->GetCombat()->OnAnimCancel.BindUObject(this, &UShooterAnimInstance::ReceiveAnimStop);
		OnMontageEnded.AddDynamic(this, &UShooterAnimInstance::ReceiveMontageEnded);
		OnMontageStarted.AddDynamic(this, &UShooterAnimInstance::ReciveMontageStarted);		
	}
}

void UShooterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if (Shooter)
	{
		bIsFalling = !Shooter->GetShooterMovement()->bIsOnGround;
			
		if (Shooter->GetInput()->bIsMoving)
		{
			AnalogModifier = Shooter->GetInput()->bIsRunning ? 2.f : 1.f;
		}

		else
		{
			AnalogModifier = 0.f;
		}

		if (AlphaIK > 0.f)
		{
			ComputeWeaponSway(DeltaSeconds);
		}

		HeadBoneRotation = Shooter->GetAnchor()->GetRelativeRotation();
	}
}

void UShooterAnimInstance::NativeUninitializeAnimation()
{
	if (Shooter)
	{
		Shooter->GetCombat()->OnLocomotionChange.Remove(LocomotionHandle);
		Shooter->GetCombat()->OnAimStart.Unbind();
		Shooter->GetCombat()->OnAimStop.Unbind();
		Shooter->GetCombat()->OnReload.Unbind();
		Shooter->GetCombat()->OnSwap.Unbind();
		Shooter->GetCombat()->OnAnimCancel.Unbind();

		//preferably find a better way to get a handle to a dynamic multicast
		OnMontageEnded.Remove(this, TEXT("ReceiveMontageEnded"));
		OnMontageStarted.Remove(this, TEXT("ReciveMontageStarted"));
	}
}

void UShooterAnimInstance::ComputeWeaponSway(const float DeltaSeconds)
{
	//Computes rotation for the right hand when a weapon is held
	REffectorRotation.Roll = FMath::FInterpTo(REffectorRotation.Roll, Shooter->GetInput()->MoveX * MoveSwayMultiplier, DeltaSeconds, SwaySpeed);
	REffectorRotation.Pitch = FMath::FInterpTo(REffectorRotation.Pitch, Shooter->GetInput()->LookY * LookSwayMultiplier, DeltaSeconds, SwaySpeed);
	REffectorRotation.Yaw = FMath::FInterpTo(REffectorRotation.Yaw, Shooter->GetInput()->LookX * LookSwayMultiplier, DeltaSeconds, SwaySpeed);

#if WITH_EDITORONLY_DATA
	if (bLiveUpdates)
	{
		HeldWeapon->GetMesh()->SetRelativeLocation(HeldWeapon->AnimData.AbsoluteLocationOffset);
		HeldWeapon->GetMesh()->SetRelativeRotation(HeldWeapon->AnimData.AbsoluteRotationOffset);

		REffectorLocation = HeldWeapon->AnimData.REffectorLocationOffset;
		RTargetLocation = HeldWeapon->AnimData.RTargetLocationOffset;
		LTargetLocation = HeldWeapon->AnimData.LTargetLocationOffset;
	}
#endif

	//Computes IK rotation and position for left effector
	FTransform Transform = HeldWeapon->GetMesh()->GetSocketTransform(UCombatComponent::SecondarySocketName);
	LEffectorLocation = Transform.GetLocation();
	LEffectorRotation = Transform.GetRotation().Rotator();

	//Compute impulse
	ImpulseLocation = FVector(-1.f * HeldWeapon->AnimData.ImpulseOffset, 0.f, 0.f);
	REffectorRotation.Pitch += HeldWeapon->AnimData.ImpulseOffset * HeldWeapon->AnimData.ImpulseKickFactor;
}

void UShooterAnimInstance::ReceiveAimStart()
{
	Montage_Play(AimStartAnimMontage, 1.0f);
}

void UShooterAnimInstance::ReceiveAimStop()
{
	Montage_Play(AimStopAnimMontage, 1.0f);
}

void UShooterAnimInstance::ReceiveReload()
{
	Montage_Play(ReloadAnimMontage, 1.0f);
}

void UShooterAnimInstance::ReceiveAnimStop()
{
	StopAllMontages(0.1); //hack magic numba
}

void UShooterAnimInstance::ReceiveSwap()
{
	Montage_Play(SwapAnimMontage, 1.0f);
}

void UShooterAnimInstance::ReciveMontageStarted(UAnimMontage* Montage)
{
	if (Montage == MeleeAttackMontage)
	{
		Shooter->GetCombat()->MarkInAnimation();
		AlphaIK = 0;
	}

	else if (Montage == SwapAnimMontage)
	{
		AlphaIK = 0.f;
	}
}

void UShooterAnimInstance::ReceiveMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Shooter->GetCombat()->GetHeldWeapon() != nullptr)
	{
		AlphaIK = 1.f;
	}
	
	if (Montage == VaultAnimMontage)
	{

	}

	else if (Montage == ReloadAnimMontage)
	{
		Shooter->GetCombat()->ReceiveReloadComplete(bInterrupted);
	}

	else if (Montage == SwapAnimMontage)
	{
		if (bInterrupted && !Montage_IsActive(SwapAnimMontage))
		{
			Shooter->GetCombat()->ReceiveSwapComplete();
		}

		else if (!bInterrupted)
		{
			Shooter->GetCombat()->ReceiveSwapComplete();
		}
	}

	else if (Montage == AimStartAnimMontage || Montage == AimStopAnimMontage || Montage == MeleeAttackMontage)
	{
		if (!bInterrupted)
		{
			Shooter->GetCombat()->ReceiveAnimationComplete();
		}
	}
}