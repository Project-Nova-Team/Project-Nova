#pragma once

#include "CoreMinimal.h"
#include "Gun.h"
#include "FlameThrower.generated.h"

UCLASS()
class PROJECTNOVA_API AFlameThrower : public AGun
{
	GENERATED_BODY()
	
public:
	AFlameThrower();

protected:

	/** Particle system component responsible for playing flame vfx*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Flame Thrower")
	class UParticleSystemComponent* FlameSystemComponent;

	/** Audio component responsible for playing flame sfx*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Flame Thrower")
	class UAudioComponent* FlameAudio;

	/** The particle system we spawn when the flame of the flame thrower makes contact */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flame Thrower")
	class UParticleSystem* SparkSystem;

	/** Sound clip that plays when sparks are active*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flame Thrower")
	class USoundBase* SparkAudio;

	/** Is the player able to pick this flame thrower up?*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flame Thrower")
	uint8 bLootable : 1;

private:

	/** Temp instance of spark system*/
	class UParticleSystemComponent* SparkSystemComponent;

	/** Temp instance of spark audio system*/
	class UAudioComponent* SparkAudioComponent;

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
