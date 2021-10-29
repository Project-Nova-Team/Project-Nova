
#include "BarkTrigger.h"
#include "Blueprint/UserWidget.h"


// Sets default values
ABarkTrigger::ABarkTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void ABarkTrigger::ShowUIPrompt()
{
	// default for now
	ShowBarkEvent.Broadcast("Press", "To Jump");
}

void ABarkTrigger::HideUIPrompt()
{
	HideBarkEvent.Broadcast();
}

void ABarkTrigger::EnterOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ShowUIPrompt();
}

void ABarkTrigger::ExitOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	HideUIPrompt();
}

