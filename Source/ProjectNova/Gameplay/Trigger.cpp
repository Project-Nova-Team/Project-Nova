#include "Trigger.h"
#include "Components/BoxComponent.h"
#include "Components/BillboardComponent.h"
#include "UObject/ConstructorHelpers.h"

//This hard restricts these triggers to be project sensitive material but removing this is effectively zero cost
#include "../Player/Shooter.h"
#include "../AI/BaseAI.h"

ATrigger::ATrigger()
{
	TriggerVolume = CreateDefaultSubobject<UBoxComponent>("Trigger");
	TriggerVolume->SetCollisionProfileName("Trigger");
	SetRootComponent(TriggerVolume);

	bIsActive = true;
	bActivatedByEngi = true;

#if WITH_EDITORONLY_DATA
	SpriteComponent = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("Sprite"));
	SpriteComponent->AttachToComponent(TriggerVolume, FAttachmentTransformRules::KeepRelativeTransform);
	if (SpriteComponent)
	{
		// Structure to hold one-time initialization
		struct FConstructorStatics
		{
			ConstructorHelpers::FObjectFinderOptional<UTexture2D> TriggerTextureObject;
			FName ID_Triggers;
			FText NAME_Triggers;
			FConstructorStatics()
				: TriggerTextureObject(TEXT("/Engine/EditorResources/S_Trigger"))
				, ID_Triggers(TEXT("Triggers"))
				, NAME_Triggers(NSLOCTEXT("SpriteCategory", "Triggers", "Triggers"))
			{
			}
		};
		static FConstructorStatics ConstructorStatics;

		SpriteComponent->Sprite = ConstructorStatics.TriggerTextureObject.Get();
		SpriteComponent->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
		SpriteComponent->bHiddenInGame = true;
		SpriteComponent->SpriteInfo.Category = ConstructorStatics.ID_Triggers;
		SpriteComponent->SpriteInfo.DisplayName = ConstructorStatics.NAME_Triggers;
		SpriteComponent->bIsScreenSizeScaled = true;
	}
#endif
}

void ATrigger::BeginPlay()
{
	Super::BeginPlay();
	
	OnActorBeginOverlap.AddDynamic(this, &ATrigger::BeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &ATrigger::EndOverlap);
}

void ATrigger::BeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (bIsActive && IsValidActivator(OtherActor))
	{
		ExecuteTrigger(Cast<APawn>(OtherActor));
	}
}

void ATrigger::EndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (IsValidActivator(OtherActor))
	{
		OnTriggerExited.Broadcast(Cast<APawn>(OtherActor));
		TriggerExited(Cast<APawn>(OtherActor));
	}
}

void ATrigger::ExecuteTrigger(APawn* Sender)
{
	//Invokes blueprint logic event
	OnTriggerActivated.Broadcast(Sender);
	TriggerActivated(Sender);

	//Child classes fill out logic here
	if (!bTriggerMoreThanOnce)
	{
		SetActive(false);
	}
}

bool ATrigger::IsValidActivator(AActor* Activator) const 
{
	return (bActivatedByEngi && Activator->IsA(AShooter::StaticClass())) || (bActivatedByAI && Activator->IsA(ABaseAI::StaticClass()));
}

void ATrigger::SetActive(const bool Value)
{
	if (bIsActive != Value)
	{
		bIsActive = Value;

		SetActiveInternal();
	}
}

void ATrigger::SetActiveInternal()
{
	if (bIsActive)
	{
		TriggerVolume->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}

	else
	{
		TriggerVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

#if WITH_EDITOR
void ATrigger::PostEditChangeProperty(FPropertyChangedEvent& Event)
{
	Super::PostEditChangeProperty(Event);

	const FName PropName = Event.GetPropertyName();

	if (PropName != NAME_None)
	{
		if (PropName == GET_MEMBER_NAME_CHECKED(ATrigger, bIsActive))
		{
			SetActiveInternal();
		}
	}
}
#endif