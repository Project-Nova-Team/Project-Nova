#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "../Weapon/Weapon.h"
#include "ShooterAnimInstance.generated.h"

class AGun;

UCLASS()
class PROJECTNOVA_API UShooterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	UShooterAnimInstance();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* MeleeAttackMontage;

protected:

	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	class AShooter* Shooter;

	/** Kind of hacky, set as true to enable event reporting (use this on the arms only*/
	UPROPERTY(BlueprintReadWrite, Category = "Animation")
	uint8 bReportEvents : 1;


	/***	Locomotion		***/

	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	uint8 bUseDefaultLocomotion : 1;

	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	uint8 bUsePistolLocomotion : 1;

	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	uint8 bUseRifleLocomotion : 1;

	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	uint8 bIsFalling : 1;

	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	float AnalogModifier;

	/***	IK	   ***/

	/** Amount of sway applied from movement*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float MoveSwayMultiplier;

	/** Amount of sway to applied from look*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float LookSwayMultiplier;

	/** How quickly is sway interpolated*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float SwaySpeed;

	/** Component space position of the right hand effector*/
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	FVector REffectorLocation;

	/** Component space transform of right target (right elbow)*/
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	FVector RTargetLocation;

	/** Component space transform of left target (left elbow)*/
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	FVector LTargetLocation;

	/** World space location of left effector, determined by weapon's Secondary_Socket world space position*/
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	FVector LEffectorLocation;

	/** Orientation to rotate right effector, determined by properties such as socket orientation and weapon say*/
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	FRotator REffectorRotation;

	/** Orientation to rotate left effector, determined by weapon's Secondary_Socket orientation*/
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	FRotator LEffectorRotation;

	/** Additional effector offset determined by impulse applied from gun fire*/
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	FVector ImpulseLocation;

	/** IK Blending Alpha. 1 when weapon is held, 0 if otherwise or when in montage*/
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	float AlphaIK;

	/***	Montage		***/

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* VaultAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* SwapAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* ReloadAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* AimStartAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* AimStopAnimMontage;

private:

	/** Delegate handle for unbinding locomotion multicast delegate*/
	FDelegateHandle LocomotionHandle;

	/** Currently held weapon*/
	const AWeapon* HeldWeapon;

public:

	/*** Begin AnimInstance Interface***/

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeUninitializeAnimation() override;

private:

	void ReceiveWeaponSwitch(const AWeapon* NewWeapon);

	void ReceiveAimStart();
	void ReceiveAimStop();
	void ReceiveReload();
	void ReceiveAnimStop();
	void ReceiveSwap();

	void ComputeWeaponSway(const float DeltaSeconds);

	UFUNCTION()
	void ReceiveMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void ReciveMontageStarted(UAnimMontage* Montage);
};
