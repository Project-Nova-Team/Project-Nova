// Fill out your copyright notice in the Description page of Project Settings.
#include "Animation/AnimMontage.h"
#include "ShooterAnimInstance.h"

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
	}

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

void UShooterAnimInstance::StartVaultAnimation()
{
	// broadcast an event here that will play an anim montage in shooter blueprint!
	OnVaultPress.Broadcast();
}

void UShooterAnimInstance::PlayVaultMontage()
{
	Montage_Play(VaultAnimMontage, 1.0f);
}

void UShooterAnimInstance::OnMontageEndMethod(UAnimMontage* Montage, bool bInterupted)
{
	if(Shooter->GetStateMachine() != nullptr)
		Shooter->GetStateMachine()->GetActiveState()->FlagTransition("Walking");
}