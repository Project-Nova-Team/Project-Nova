#include "QuickTimeManager.h"
#include "../Player/Shooter.h"
#include "../AI/BaseAI.h"
#include "Components/InputComponent.h"
#include "../ShooterGameMode.h"
#include "../Slate/QuickTimeWidget.h"
#include "../ShooterController.h"
#include "../ShooterHUD.h"
#include "Components/Image.h"

AQuickTimeManager::AQuickTimeManager()
{
	ActionTime = 6.f;
	InputsRequired = 15;
}

void AQuickTimeManager::Init()
{
	AShooterGameMode* GM = GetWorld()->GetAuthGameMode<AShooterGameMode>();

	if (GM)
	{
		Shooter = GM->GetShooter();
		QuickTimeWidget = GM->GetShooterController()->ShooterHUD->GetQTWidget();
	}
}

bool AQuickTimeManager::StartQuickTime(class ABaseAI* InstigatingAI, int32 EventCount)
{
	//If an event is already active, fail to start a new one
	if (bActive || EventCount < 1 || QuickTimeInputs.Num() == 0)
	{
		return false;
	}

	//Setup
	AI = InstigatingAI;
	RequiredActions = EventCount;
	bActive = true;

	Shooter->SetStateOverride("Empty");
	Shooter->QuickTimeEventStarted(AI);

	//Select the first input type, random if this is a one shot quick time, first index if otherwise
	ActiveIndex = EventCount == 1 ? FMath::RandRange(0, QuickTimeInputs.Num() - 1) : 0;

	BindingHandle = Shooter->InputComponent->BindAction
		(QuickTimeInputs[ActiveIndex].ActionName, IE_Pressed, this, &AQuickTimeManager::ReceiveInput).GetHandle();

	QuickTimeWidget->KeyImage->SetBrushFromTexture(QuickTimeInputs[ActiveIndex].IdleTexture);
	QuickTimeWidget->SetVisibility(ESlateVisibility::Visible);
	
	//Set the first timer for when the action completes
	GetWorldTimerManager().SetTimer(ActionHandle, this, &AQuickTimeManager::DetermineActionResult, ActionTime, false);
	GetWorldTimerManager().SetTimer(WidgetHandle, this, &AQuickTimeManager::ToggleTexture, WidgetToggleTime, true);

	return true;
}

void AQuickTimeManager::StartNextAction()
{
	if (bActive && bAwaitingNextAction)
	{
		ActiveIndex = ActionCount;
		bAwaitingNextAction = false;

		BindingHandle = Shooter->InputComponent->BindAction
			(QuickTimeInputs[ActiveIndex].ActionName, IE_Pressed, this, &AQuickTimeManager::ReceiveInput).GetHandle();

		QuickTimeWidget->KeyImage->SetBrushFromTexture(QuickTimeInputs[ActiveIndex].IdleTexture);
		QuickTimeWidget->SetVisibility(ESlateVisibility::Visible);

		GetWorldTimerManager().SetTimer(ActionHandle, this, &AQuickTimeManager::DetermineActionResult, ActionTime, false);
		GetWorldTimerManager().SetTimer(WidgetHandle, this, &AQuickTimeManager::ToggleTexture, WidgetToggleTime, true);
	}
}

void AQuickTimeManager::FinishQuickTimeEvent()
{
	bActive = false;
	bAwaitingNextAction = false;
	ActionCount = 0;
	AISuccesses = 0;
	ShooterSuccesses = 0;

	Shooter->SetStateOverride("Walking");

	//this tells the AI BT the task is done
	OnQuickTimeComplete.Broadcast();
}

void AQuickTimeManager::DetermineActionResult()
{
	//Clear the input binding
	Shooter->InputComponent->RemoveActionBindingForHandle(BindingHandle);
	

	//Hide widget
	GetWorldTimerManager().ClearTimer(WidgetHandle);
	QuickTimeWidget->SetVisibility(ESlateVisibility::Collapsed);

	bool bShooterSucceeded = InputCount >= InputsRequired;
	bool bLastAction = ++ActionCount == RequiredActions;

	if (bShooterSucceeded)
	{
		++ShooterSuccesses;
	}

	else
	{
		++AISuccesses;
	}

	//If this wasn't the last action, flag that we are waiting for the next one
	bAwaitingNextAction = !bLastAction;
	InputCount = 0;

	//Inform both parties the action was completed, and the result
	AI->QuickTimeActionComplete(!bShooterSucceeded, bLastAction, AISuccesses);
	Shooter->QuickTimeActionComplete(bShooterSucceeded, bLastAction, ShooterSuccesses);
}

void AQuickTimeManager::ToggleTexture()
{
	bTextureActive = !bTextureActive;

	UTexture2D* ToggledTexture = bTextureActive ? QuickTimeInputs[ActiveIndex].ActiveTexture : QuickTimeInputs[ActiveIndex].IdleTexture;
	QuickTimeWidget->KeyImage->SetBrushFromTexture(ToggledTexture);
}

void AQuickTimeManager::ReceiveInput()
{
	InputCount++;
}