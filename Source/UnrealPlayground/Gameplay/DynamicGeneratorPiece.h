#pragma once

#include "CoreMinimal.h"
#include "GeneratorPiece.h"
#include "DynamicGeneratorPiece.generated.h"

class UDelayedActionManager;
struct FDelayedActionHandle;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFlickerEvent);

/**
 * This class exists because I have not yet added blueprint functionality to the DelayedActionManager
 * In order for us to time flickering on the broken light material we need keep track of time so we can have
 * better fidelity in the visuals
 * 
 * Note that the actual texture swapping occurs in blueprint becuase I don't know how to create a dynamic material
 * in cpp. It fights me..
 */
UCLASS()
class UNREALPLAYGROUND_API ADynamicGeneratorPiece : public AGeneratorPiece
{
	GENERATED_BODY()

public:
	ADynamicGeneratorPiece();

	void BreakPiece() override;

protected:

	/** Event responsible for switching between two textures*/
	UPROPERTY(BlueprintAssignable)
	FFlickerEvent OnFlicker;

	UPROPERTY(Category = Generator, EditAnywhere, BlueprintReadWrite)
	UTexture2D* DisabledTexture;

	UPROPERTY(Category = Generator, EditAnywhere, BlueprintReadWrite)
	UTexture2D* EnabledTexture;

	/** How many times does this light flicker each flicker iteration*/
	UPROPERTY(Category = Generator, EditAnywhere, BlueprintReadWrite)
	int32 FlickerCount;

	/** How many seconds are in between each flicker session*/
	UPROPERTY(Category = Generator, EditAnywhere, BlueprintReadWrite)
	float FlickerRest;

	/** How many seconds are in between each pulse*/
	UPROPERTY(Category = Generator, EditAnywhere, BlueprintReadWrite)
	float PulseRest;

	/** This bool helps flipping the texture in BP easier*/
	UPROPERTY(Category = Generator, EditAnywhere, BlueprintReadWrite)
	uint8 bIsFlickeredOn : 1;

	UFUNCTION(BlueprintCallable)
	void StopFlicker();

	void BeginPlay() override;

	void Flicker();

	int32 CurrentFlickerCount;

private:
	UDelayedActionManager* Delay;

	FDelayedActionHandle* Handle;
};
