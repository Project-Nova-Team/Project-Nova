#pragma once

#include "CoreMinimal.h"
#include "Gun.h"
#include "FlameThrower.generated.h"

class UParticleSystemComponent;
class UParticleSystem;
class UAudioComponent;

UCLASS()
class PROJECTNOVA_API AFlameThrower : public AGun
{
	GENERATED_BODY()
	
public:
	AFlameThrower();

protected:

	/** Audio component responsible for playing flame sfx*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Flame Thrower")
	UAudioComponent* FlameAudio;

	/** The particle system we spawn when the flame of the flame thrower makes contact */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flame Thrower")
	UParticleSystem* FlameSystem;

	/** The particle system we spawn when the flame of the flame thrower makes contact */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flame Thrower")
	UParticleSystem* SparkSystem;

	/** Sound clip that plays when sparks are active*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flame Thrower")
	class USoundBase* SparkAudio;

	/** Is the player able to pick this flame thrower up?*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flame Thrower")
	uint8 bLootable : 1;

private:

	/** Temp instance of flame system*/
	TWeakObjectPtr<UParticleSystemComponent> FlameSystemComponent;

	/** Temp instance of spark system*/
	TWeakObjectPtr<UParticleSystemComponent> SparkSystemComponent;

	/** Temp instance of spark audio system*/
	TWeakObjectPtr<UAudioComponent> SparkAudioComponent;

	/** True if the flame thrower is firing. Used to manage flame system activation*/
	uint8 bAttacking : 1;

	/** True if the flame thrower hit something last check. Used to manage spark system activation*/
	uint8 bHitLastFrame : 1;

protected:

	/** Blueprint hook for AI usage. Plays particle systems/sounds*/
	UFUNCTION(BlueprintCallable, Category = "Flame Thrower")
	void StartFlamethrower();

	/** Blueprint hook for AI usage. Stops particle systems/sounds*/
	UFUNCTION(BlueprintCallable, Category = "Flame Thrower")
	void StopFlamethrower();

	virtual bool IsAimable() override { return false; }
	virtual bool IsReloadable() override { return false; }
	
	virtual bool CanInteract() const override { return bLootable; }

	virtual void Attack() override;
	virtual void StopAttack() override;
};
