#include "GormAnimInstance.h"
#include "../AI/BaseAI.h"

void UGormAnimInstance::NativeInitializeAnimation()
{
	Gorm = Cast<ABaseAI>(GetOwningActor());

	if (Gorm)
	{
		OnMontageEnded.AddDynamic(this, &UGormAnimInstance::ReceiveMontageEnded);
		OnMontageStarted.AddDynamic(this, &UGormAnimInstance::ReceiveMontageStarted);
	}
}

void UGormAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{

}

void UGormAnimInstance::NativeUninitializeAnimation()
{
	if (Gorm)
	{
		OnMontageEnded.Remove(this, TEXT("ReceiveMontageEnded"));
		OnMontageStarted.Remove(this, TEXT("ReceiveMontageStarted"));
	}
}

void UGormAnimInstance::ReceiveMontageStarted(UAnimMontage* Montage)
{	
	if (Montage->SlotAnimTracks.Num() < 1)
	{
		return;
	}

	if (Montage->SlotAnimTracks[0].SlotName == TEXT("UpperBody"))
	{
		bPlayingUpperBody = true;
	}
}

void UGormAnimInstance::ReceiveMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	bPlayingUpperBody = false;
}
