#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GormAnimInstance.generated.h"

UCLASS()
class PROJECTNOVA_API UGormAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	class ABaseAI* Gorm;

	/** True if a montage is playing for only the upper body*/
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	uint8 bPlayingUpperBody : 1;

protected:

	UFUNCTION()
	void ReceiveMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void ReceiveMontageStarted(UAnimMontage* Montage);

	///		Anim instance interface		///
	void NativeInitializeAnimation() override;
	void NativeUpdateAnimation(float DeltaSeconds) override;
	void NativeUninitializeAnimation() override;
};
