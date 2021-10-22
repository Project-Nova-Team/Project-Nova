// Fill out your copyright notice in the Description page of Project Settings.
#include "ShooterAnimInstance.h"
#include "Animation/AnimMontage.h"


UShooterAnimInstance::UShooterAnimInstance()
{
}

UShooterAnimInstance::~UShooterAnimInstance()
{
}

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
	else
	{
		ShooterMovement = Shooter->GetShooterMovement();
		ShooterMesh = Shooter->GetSkeletalMeshComponent();
		ShooterCombat = Shooter->GetCombat();
	}

	// Binds Montage end method to an in-engine dynamic multicast delegate named OnMontageEnded
	OnMontageEnded.AddDynamic(this, &UShooterAnimInstance::OnMontageEndMethod);
}

bool UShooterAnimInstance::IsWalking()
{
	return ShooterMovement->bIsOnGround && !ShooterMovement->InputVelocity.IsNearlyZero();
}

bool UShooterAnimInstance::IsFalling()
{
	return !ShooterMovement->bIsOnGround;
}

void UShooterAnimInstance::BroadcastVaultEvent()
{
	// broadcast an event here that will play an anim montage in shooter blueprint!
	OnVaultPress.Broadcast();
}

void UShooterAnimInstance::PlayVaultMontage()
{
	Montage_Play(VaultAnimMontage, 1.0f);
}

void UShooterAnimInstance::PlaySwapMontage()
{
	Montage_Play(SwapAnimMontage, 1.0f);
}

void UShooterAnimInstance::PlayReloadMontage()
{
	Montage_Play(ReloadAnimMontage, .3f);
}

void UShooterAnimInstance::OnMontageEndMethod(UAnimMontage* Montage, bool bInterupted)
{
	// If the montage is vault or swap, go to walk anim on end. 
	if (Montage == VaultAnimMontage || SwapAnimMontage)
	{
		UShooterStateMachine* ShooterStateMachine = Shooter->GetStateMachine();

		if (ShooterStateMachine != nullptr)
			ShooterStateMachine->GetActiveState()->FlagTransition("Walking");

		ShooterCombat->ResetLockout();
	}
}