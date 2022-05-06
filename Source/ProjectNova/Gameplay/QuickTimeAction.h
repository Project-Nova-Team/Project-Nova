#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "QuickTimeAction.generated.h"

class AQuickTimeManager;

USTRUCT(BlueprintType)
struct FQuickTimeInput
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName ActionName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* IdleTexture;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* ActiveTexture;
};

UCLASS()
class PROJECTNOVA_API UQuickTimeAction : public UObject
{
	GENERATED_BODY()

	friend class AQuickTimeManager;

public:

	/** Input required for this action*/
	UPROPERTY(EditAnywhere, Category = "Quick Time | Input")
	FQuickTimeInput RequiredInput;

	/** Number of presses required for a success during the action*/
	UPROPERTY(EditAnywhere, Category = "Quick Time | Input")
	int32 InputsRequired;

	/** Montage played by the player during the input phase*/
	UPROPERTY(EditAnywhere, Category = "Quick Time | Animation")
	UAnimMontage* PlayerStruggle;

	/** Montage played by player when succeeding quick time action*/
	UPROPERTY(EditAnywhere, Category = "Quick Time | Animation")
	UAnimMontage* PlayerSuccess;

	/** Montage played by player when failing quick time action*/
	UPROPERTY(EditAnywhere, Category = "Quick Time | Animation")
	UAnimMontage* PlayerFail;

	/** Montage played by the enemy during the input phase*/
	UPROPERTY(EditAnywhere, Category = "Quick Time | Animation")
	UAnimMontage* EnemyStruggle;

	/** Montage played by enemy when player succeeds quick time action*/
	UPROPERTY(EditAnywhere, Category = "Quick Time | Animation")
	UAnimMontage* EnemySuccess;

	/** Montage played by enemy when player succeeds quick time action*/
	UPROPERTY(EditAnywhere, Category = "Quick Time | Animation")
	UAnimMontage* EnemyFail;

	/** Amount of damage dealt when the player passes the quick time event*/
	UPROPERTY(EditAnywhere, Category = "Quick Time | Damage")
	float DamageOnSuccess;

	/** Amount of damage taken when the player fails the quick time event*/
	UPROPERTY(EditAnywhere, Category = "Quick Time | Damage")
	float DamageOnFail;

protected:

	/** Scriptable event for when this action starts*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Quick Time", meta = (DisplayName = "Struggle Started"))
	void ReceiveStruggleStart();

	/** Scriptable event for when the struggle portion of this action ends*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Quick Time", meta = (DisplayName = "Struggle Ended"))
	void ReceiveStruggleEnd(bool bPlayerSucceeded);

	/** Scriptable event for when the result portion of this action ends*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Quick Time", meta = (DisplayName = "Action Ended"))
	void ReceiveActionEnd(bool bPlayerSucceeded);
};
