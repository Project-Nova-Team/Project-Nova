#include "Trigger.h"
#include "Components/BoxComponent.h"
#include "Components/BillboardComponent.h"
#include "UObject/ConstructorHelpers.h"

ATrigger::ATrigger()
{
	TriggerVolume = CreateDefaultSubobject<UBoxComponent>("Trigger");
	TriggerVolume->SetCollisionProfileName("OverlapOnlyPawn");
	SetRootComponent(TriggerVolume);

	bStartActive = true;
	bIsActive = true;


	//Yoinked from source
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
		SpriteComponent->bHiddenInGame = false;
		SpriteComponent->SpriteInfo.Category = ConstructorStatics.ID_Triggers;
		SpriteComponent->SpriteInfo.DisplayName = ConstructorStatics.NAME_Triggers;
		SpriteComponent->bIsScreenSizeScaled = true;
	}
#endif
}

void ATrigger::BeginPlay()
{
	Super::BeginPlay();

	if (TriggerWhiteList.Num() == 0)
	{
		FString Log;
		GetName(Log);
		Log += TEXT(" does not have any actors attached to the class white list so it will never trigger! Assign pawn subclasses in blueprint");
		UE_LOG(LogTemp, Error, TEXT("%s"), *Log);

		bStartActive = false;
	}

	SetActive(bStartActive);
	
	OnActorBeginOverlap.AddDynamic(this, &ATrigger::BeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &ATrigger::EndOverlap);

#if WITH_EDITORONLY_DATA
	SpriteComponent->SetHiddenInGame(true);
#endif
}

void ATrigger::BeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (bIsActive && TriggerWhiteList.Contains(OtherActor->GetClass()))
	{
		//Forces other actor to be a pawn type. This will cause crashes if other actor is not a pawn
		ExecuteTrigger(Cast<APawn>(OtherActor));
	}
}

void ATrigger::EndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (TriggerWhiteList.Contains(OtherActor->GetClass()))
	{
		OnTriggerExited.Broadcast(Cast<APawn>(OtherActor));
	}
}

void ATrigger::ExecuteTrigger(APawn* Sender)
{
	//Invokes blueprint logic event
	OnTriggerActivated.Broadcast(Sender);

	//Child classes fill out logic here
	if (!bTriggerMoreThanOnce)
	{
		SetActive(false);
	}
}

void ATrigger::SetActive(const bool Value)
{
	if (bIsActive != Value)
	{
		bIsActive = Value;

		if (bIsActive)
		{
			TriggerVolume->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);			
		}

		else
		{
			TriggerVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}