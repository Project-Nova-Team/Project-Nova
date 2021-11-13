#include "DynamicGeneratorPiece.h"
#include "../ShooterGameMode.h"
#include "../Utility/DelayedActionManager.h"

ADynamicGeneratorPiece::ADynamicGeneratorPiece()
{
	FlickerCount = 6;
	PulseRest = 0.25;
	FlickerRest = 6;
}

void ADynamicGeneratorPiece::BeginPlay()
{
	Super::BeginPlay();

	Delay = GetWorld()->GetAuthGameMode<AShooterGameMode>()->GetDelayedActionManager();
	Handle = Delay->StartDelayedAction(this, &ADynamicGeneratorPiece::Flicker, PulseRest);
}

void ADynamicGeneratorPiece::Flicker()
{
	OnFlicker.Broadcast();
	CurrentFlickerCount++;

	if (CurrentFlickerCount == FlickerCount)
	{
		CurrentFlickerCount = 0;
		Handle = Delay->StartDelayedAction(this, &ADynamicGeneratorPiece::Flicker, FlickerRest);
	}

	else
	{
		Handle = Delay->StartDelayedAction(this, &ADynamicGeneratorPiece::Flicker, PulseRest);
	}
}

void ADynamicGeneratorPiece::StopFlicker()
{
	Handle->GetAction()->StopAction();
	Handle = nullptr; // dont want a stale reference
}