#include "QuickTimeManager.h"
#include "../Player/Shooter.h"
#include "../AI/BaseAI.h"
#include "Components/InputComponent.h"
#include "../ShooterGameMode.h"
#include "../Slate/QuickTimeWidget.h"
#include "../ShooterController.h"
#include "../ShooterHUD.h"
#include "Components/Image.h"
#include "../Animation/ShooterCutscene.h"
#include "Animation/AnimMontage.h"
#include "Components/CapsuleComponent.h"

void AQuickTimeManager::Init()
{
	if (AShooterGameMode* GM = GetWorld()->GetAuthGameMode<AShooterGameMode>())
	{
		Shooter = GM->GetShooter();
		QuickTimeWidget = GM->GetShooterController()->ShooterHUD->GetQTWidget();
		Cutscene = GM->GetShooterCutscene();
	}
}

FTransform AQuickTimeManager::ComputeQuickTimeLocation()
{
	FTransform Transform;

	FVector Location = AI->GetActorLocation();
	const FVector Offset = 
		AI->GetActorForwardVector() * 
		(AI->GetCapsuleComponent()->GetScaledCapsuleRadius() + Shooter->GetCollider()->GetScaledCapsuleRadius());

	Location.Z -= AI->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	Location.Z += Shooter->GetCollider()->GetScaledCapsuleHalfHeight();
	Location += Offset;

	Transform.SetLocation(Location);
	Transform.SetRotation((-AI->GetActorForwardVector()).ToOrientationQuat());

	return Transform;
}

bool AQuickTimeManager::StartQuickTime(class ABaseAI* InstigatingAI)
{
	//If an event is already active, fail to start a new one
	if (bActive || InstigatingAI->QuickTimeActions.Num() < 1)
	{
		return false;
	}

	//Setup
	AI = InstigatingAI;
	bActive = true;

	Cutscene->StartCinematic(ComputeQuickTimeLocation());
	StartAction();

	return true;
}

void AQuickTimeManager::StartAction()
{
	CurrentAction =  NewObject<UQuickTimeAction>(this, AI->QuickTimeActions[ActionIndex].Get());
	
	BindingHandle = Shooter->InputComponent->BindAction(CurrentAction->RequiredInput.ActionName, IE_Pressed, this, &AQuickTimeManager::ReceiveInput).GetHandle();

	Cutscene->PlayAnimation(CurrentAction->PlayerStruggle);
	AI->PlayAnimMontage(CurrentAction->EnemyStruggle);
	
	GetWorldTimerManager().SetTimer(ActionHandle, this, &AQuickTimeManager::CompleteStruggle, CurrentAction->PlayerStruggle->GetPlayLength());
}

void AQuickTimeManager::CompleteStruggle()
{
	Shooter->InputComponent->RemoveActionBindingForHandle(BindingHandle);
	float AnimTime;

	bCurrentActionResult = InputCount >= CurrentAction->InputsRequired;
	CurrentAction->ReceiveStruggleEnd(bCurrentActionResult);
	
	//Player succeeded during the struggle
	if (bCurrentActionResult)
	{	
		Cutscene->PlayAnimation(CurrentAction->PlayerSuccess);
		AI->PlayAnimMontage(CurrentAction->EnemySuccess);

		AnimTime = CurrentAction->PlayerSuccess->GetPlayLength();
	}

	//Player failed
	else
	{
		Cutscene->PlayAnimation(CurrentAction->PlayerFail);
		AI->PlayAnimMontage(CurrentAction->EnemyFail);

		AnimTime = CurrentAction->PlayerFail->GetPlayLength();
	}

	GetWorldTimerManager().SetTimer(ActionHandle, this, &AQuickTimeManager::CompleteResult, AnimTime);
}

void AQuickTimeManager::CompleteResult()
{
	CurrentAction->ReceiveActionEnd(bCurrentActionResult);

	if (bCurrentActionResult && CurrentAction->DamageOnSuccess > 0.f)
	{
		AI->TakeDamage(CurrentAction->DamageOnSuccess, FDamageEvent(), Shooter->GetController(), Shooter);
	}

	else if(CurrentAction->DamageOnFail > 0.f)
	{
		Shooter->TakeDamage(CurrentAction->DamageOnFail, FDamageEvent(), AI->GetController(), AI);
	}

	if (ActionIndex + 1 < AI->QuickTimeActions.Num())
	{
		++ActionIndex;
		StartAction();	
	}

	else
	{
		bActive = false;
		ActionIndex = 0;
		Cutscene->EndCinematic(TEXT("Walking"));
	}
}

void AQuickTimeManager::ReceiveInput()
{
	InputCount++;
}

void AQuickTimeManager::ToggleTexture()
{
	bTextureActive = !bTextureActive;

	UTexture2D* ToggledTexture = bTextureActive ? CurrentAction->RequiredInput.ActiveTexture : CurrentAction->RequiredInput.IdleTexture;
	QuickTimeWidget->KeyImage->SetBrushFromTexture(ToggledTexture);
}