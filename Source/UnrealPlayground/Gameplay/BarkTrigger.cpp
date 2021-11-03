
#include "BarkTrigger.h"
#include "Blueprint/UserWidget.h"


// Sets default values
ABarkTrigger::ABarkTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ABarkTrigger::BeginPlay()
{
	Super::BeginPlay();

	// bind delegates
	OnActorBeginOverlap.AddDynamic(this, &ABarkTrigger::BeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &ABarkTrigger::EndOverlap);
}

void ABarkTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABarkTrigger::ShowUIPrompt()
{
	// this is being listened for by barktrigger blueprint
	ShowBarkEvent.Broadcast(PrefixText, SuffixText, KeyText);
}

void ABarkTrigger::HideUIPrompt()
{
	HideBarkEvent.Broadcast();
}

void ABarkTrigger::BeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if(OtherActor == TargetActor)
		ShowUIPrompt();
}

void ABarkTrigger::EndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OtherActor == TargetActor)
		HideUIPrompt();
}

