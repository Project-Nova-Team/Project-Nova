#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseAI.generated.h"

class UHealthComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FLoadEvent, const bool);

UCLASS()
class PROJECTNOVA_API ABaseAI : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseAI();

	/** Multicast delegate that broadcasts when SetIsLoaded gets called*/
	FLoadEvent OnLoadStatusChanged;

	/** Gets health component*/
	FORCEINLINE UHealthComponent* GetHealth() const { return Health; }

	/** Gets current load state*/
	UFUNCTION(BlueprintGetter)
	FORCEINLINE bool GetIsLoaded() const { return bIsLoaded; }

	/**
	 * Changes load state of the AI.
	 * This disables the AI's collision, mesh, and controller logic
	 * 
	 * @param	bLoadAI				The new load state of this AI
	 */
	UFUNCTION(BlueprintSetter)
	void SetIsLoaded(const bool bLoadAI);

protected:

	/** Health component of this AI Character*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Health)
	UHealthComponent* Health;

	/** Active load state of this AI. Unloaded AI are invisible and have no logic running*/
	UPROPERTY(EditAnywhere, BlueprintGetter = GetIsLoaded, BlueprintSetter = SetIsLoaded)
	uint8 bIsLoaded : 1;

	/** Executed when the this Character's health reaches 0*/
	UFUNCTION()
	void OnDeath();

	/** Executed when the this Character revives*/
	UFUNCTION()
	void OnRevive();

#if WITH_EDITOR
public:
	/** Lets us hide the AI when a designer marks an AI as unloaded in the details panel*/
	void PostEditChangeProperty(FPropertyChangedEvent& Event) override;
#endif

#if WITH_EDITORONLY_DATA
protected:
	UPROPERTY()
	UBillboardComponent* SpriteComponent;
#endif
};



#if WITH_EDITOR
class UTexture2D;

//Static helper class providing sprites for ai state debug
class FBaseAIDebugHelper
{

public:
	static UTexture2D* Default();
	static UTexture2D* Disabled();
	static UTexture2D* Aggressive();
	static UTexture2D* Aggravated();
};
#endif

