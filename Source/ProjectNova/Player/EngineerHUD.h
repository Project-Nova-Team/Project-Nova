#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "../Weapon/TWeapon.h"
#include "../Gameplay/InteractiveObject.h"
#include "EngineerHUD.generated.h"

class AEngineer;
class UTCombatComponent;
class UUserWidget;

UCLASS()
class PROJECTNOVA_API AEngineerHUD : public AHUD
{ 
	GENERATED_BODY()

public:

	void Initialize();

	/** Displays pause menu widget */
	void SetPauseDisplay(const bool bShowPauseMenu);

protected:

	/** The player character*/
	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	AEngineer* Engineer;

	/** The combat component attached to Engineer. Cached for ease of use in BP*/
	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	UTCombatComponent* Combat;



	/*** Blueprint defined widgets that collectively make up the HUD UI ***/

	UPROPERTY(BlueprintReadWrite, Category = "HUD")
	UUserWidget* InteractionPromptWidget;

	UPROPERTY(BlueprintReadWrite, Category = "HUD")
	UUserWidget* WeaponDisplayWidget;

	UPROPERTY(BlueprintReadWrite, Category = "HUD")
	UUserWidget* DeathScreenWidget;

	// Pause Screen Related Widgets //

	UPROPERTY(BlueprintReadWrite, Category = "HUD")
	UUserWidget* PauseMenuWidget;

	UPROPERTY(BlueprintReadWrite, Category = "HUD")
	UUserWidget* ExitConfirmationWidget;

	UPROPERTY(BlueprintReadWrite, Category = "HUD")
	UUserWidget* SettingsMenuWidget;

	UPROPERTY(BlueprintReadWrite, Category = "HUD")
	UUserWidget* ControlsMenuWidget;

protected:

	

	/** Blueprint event bound to the engineer's combat component. Fired when updates are needed.*/
	UFUNCTION(BlueprintImplementableEvent, Category = "HUD", meta = (DisplayName = "Weapon HUD Update"))
	void ReceiveWeaponUpdate(const FWeaponHUD& Info);

	/** Blueprint event that fires when we need to update Prompt data to the screen*/
	UFUNCTION(BlueprintImplementableEvent, Category = "HUD", meta = (DisplayName = "Provide Interaction"))
	void InteractionPromptProvided(const FInteractionPrompt& Prompt);

	/** Blueprint event that fires when we need to update Prompt data to the screen*/
	UFUNCTION(BlueprintImplementableEvent, Category = "HUD", meta = (DisplayName = "Revoke Interaction"))
	void InteractionPromptRevoke();

	UFUNCTION(BlueprintImplementableEvent, Category = "HUD", meta = (DisplayName = "Build Widgets"))
	void BuildBlueprintWidgets();

private:

	/** Internal function that executes relevant interaction prompt updates in blueprint*/
	void ReceiveInteractionUpdate(IInteractiveObject* Info);
};