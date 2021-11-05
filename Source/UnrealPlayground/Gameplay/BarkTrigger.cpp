
#include "BarkTrigger.h"
#include "Blueprint/UserWidget.h"


// Sets default values
ABarkTrigger::ABarkTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void ABarkTrigger::BeginPlay()
{
	Super::BeginPlay();

	// bind delegates
	OnActorBeginOverlap.AddDynamic(this, &ABarkTrigger::BeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &ABarkTrigger::EndOverlap);

	BoxTrigger = Cast<UBoxComponent>(GetComponentByClass(UBoxComponent::StaticClass()));
}

void ABarkTrigger::ShowUIPrompt()
{
	// this is being listened for by barktrigger blueprint
	ShowBarkEvent.Broadcast(PrefixText, SuffixText, KeyText);
}

void ABarkTrigger::HideUIPrompt()
{
	HideBarkEvent.Broadcast();

	// disable overlap
	if (bDisappears)
	{
		RemoveSelf(this);
	}
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

void ABarkTrigger::RemoveSelf(AActor* Actor)
{
	BoxTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABarkTrigger::RestoreSelf(AActor* Actor)
{
	BoxTrigger->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

