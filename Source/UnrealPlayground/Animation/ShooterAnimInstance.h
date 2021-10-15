#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include <UnrealPlayground/Player/Shooter.h>
#include "../State/State.h"
#include "../State/FPS/ShooterStateMachine.h"
#include "../Player/ShooterMovementComponent.h"
#include "../Weapon/CombatComponent.h"
#include "ShooterAnimInstance.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FVaultEvent);

class FOnMontageEndedMCDelegate;

UCLASS()
class UNREALPLAYGROUND_API UShooterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UShooterAnimInstance();
	~UShooterAnimInstance();

	/** Broadcasts event for vaulting animation*/
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void BroadcastVaultEvent();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* VaultAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* SwapAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	class UCameraAnim* CameraDeathAnimation;

private:

	/**Invoked when the shooter can vault and presses space*/
	UPROPERTY(BlueprintAssignable)
	FVaultEvent OnVaultPress;

	USkeletalMeshComponent* ShooterMesh;

	/** Animation Hooks**/

	/** Returns whether this shooter is walking*/
	UFUNCTION(BlueprintCallable, Category = "Animation")
	bool IsWalking();

	/** Returns whether this shooter is falling*/
	UFUNCTION(BlueprintCallable, Category = "Animation")
	bool IsFalling();

	UFUNCTION(BlueprintCallable, Category = "Animation")
	void PlayVaultMontage();

	UFUNCTION(BlueprintCallable, Category = "Animation")
	void PlaySwapMontage();

	

	/** Called when a montage ends*/
	UFUNCTION()
	void OnMontageEndMethod(UAnimMontage* Montage, bool bInterupted);

	void NativeBeginPlay() override;

protected:

	/** Shooter Reference - obtained by getting owner and casting to AShooter*/
	AShooter* Shooter;

	UShooterMovementComponent* ShooterMovement;

	UPROPERTY(BlueprintReadOnly)
	UCombatComponent* ShooterCombat;
};
