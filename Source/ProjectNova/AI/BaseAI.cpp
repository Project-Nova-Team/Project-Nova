#include "BaseAI.h"
#include "../Gameplay/HealthComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Animation/AnimInstance.h"

#if WITH_EDITOR
	#include "Components/BillboardComponent.h"
#endif

ABaseAI::ABaseAI()
{
	Health = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));
	Health->OnDeath.AddDynamic(this, &ABaseAI::OnDeath);
	Health->OnRevive.AddDynamic(this, &ABaseAI::OnRevive);

	bIsLoaded = true;
	bUseControllerRotationYaw = false;

#if WITH_EDITOR
	SpriteComponent = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("Sprite"));

	if (!IsRunningCommandlet() && (SpriteComponent != NULL))
	{
		SpriteComponent->Sprite = bIsLoaded ? FBaseAIDebugHelper::Default() : FBaseAIDebugHelper::Disabled();
		SpriteComponent->bHiddenInGame = true;
		SpriteComponent->SetVisibleFlag(true);
		SpriteComponent->SpriteInfo.Category = TEXT("AI");
		SpriteComponent->SpriteInfo.DisplayName = NSLOCTEXT("AI", "Debug", "Debug");
		SpriteComponent->SetupAttachment(RootComponent);
		SpriteComponent->SetAbsolute(false, false, true);
		SpriteComponent->bIsScreenSizeScaled = true;
		SpriteComponent->SetRelativeLocation(FVector(0, 0, GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 30.f));
	}
#endif
}

void ABaseAI::SetIsLoaded(const bool bLoadAI)
{
	GetMesh()->SetVisibility(bLoadAI);
	OnLoadStatusChanged.Broadcast(bLoadAI);

	bIsLoaded = bLoadAI;

#if WITH_EDITOR
	SpriteComponent->SetSprite(bIsLoaded ? FBaseAIDebugHelper::Default() : FBaseAIDebugHelper::Disabled());
#endif
}

void ABaseAI::OnDeath()
{
	GetMesh()->GetAnimInstance()->Montage_Play(DeathMontage);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("IgnoreOnlyPawn"));
	//GetMesh()->SetAnimationMode(EAnimationMode::AnimationCustomMode);
	//GetMesh()->SetSimulatePhysics(true);
}

void ABaseAI::OnRevive()
{
	GetMesh()->GetAnimInstance()->StopAllMontages(0.f);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));
	//GetMesh()->SetSimulatePhysics(false);
	//GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
}

#if WITH_EDITOR
void ABaseAI::PostEditChangeProperty(struct FPropertyChangedEvent& Event)
{
	Super::PostEditChangeProperty(Event);

	FName PropertyName = (Event.Property != NULL) ? Event.Property->GetFName() : NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(ABaseAI, bIsLoaded))
	{
		SetIsLoaded(bIsLoaded);
	}
}


/// Begin debug AI implementations ///

UTexture2D* FBaseAIDebugHelper::Default()
{
	return LoadObject<UTexture2D>(nullptr, TEXT("/Game/Art/Textures/DebugAI/T_DebugAI_Default_D"));
}

UTexture2D* FBaseAIDebugHelper::Disabled()
{
	return LoadObject<UTexture2D>(nullptr, TEXT("/Game/Art/Textures/DebugAI/T_DebugAI_Disabled_D"));
}

UTexture2D* FBaseAIDebugHelper::Aggressive()
{
	return LoadObject<UTexture2D>(nullptr, TEXT("/Game/Art/Textures/DebugAI/T_DebugAI_Aggressive_D"));
}

UTexture2D* FBaseAIDebugHelper::Aggravated()
{
	return LoadObject<UTexture2D>(nullptr, TEXT("/Game/Art/Textures/DebugAI/T_DebugAI_Aggravated_D"));
}
#endif