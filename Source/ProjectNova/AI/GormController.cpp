#include "GormController.h"
#include "BaseAI.h"
#include "../Gameplay/HealthComponent.h"
#include "../Weapon/DamageTypeMelee.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"

FName AGormController::HitStunKeyName = TEXT("bHitStunned");
FName AGormController::CanBeStunnedKeyName = TEXT("bCanBeStunned");

void AGormController::ReceiveTakeDamage(const class UDamageType* DamageType)
{
	if (DamageType->GetClass() == UDamageTypeMelee::StaticClass())
	{
		if (Blackboard->GetValue<UBlackboardKeyType_Bool>(CanBeStunnedKey))
		{
			Blackboard->SetValue<UBlackboardKeyType_Bool>(HitStunnedKey, true);
			Blackboard->SetValue<UBlackboardKeyType_Bool>(CanBeStunnedKey, false);
		}	
	}
}

void AGormController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	GetAICharacter()->GetHealth()->OnDamaged.AddDynamic(this, &AGormController::ReceiveTakeDamage);
	HitStunnedKey = Blackboard->GetKeyID(HitStunKeyName);
	CanBeStunnedKey = Blackboard->GetKeyID(CanBeStunnedKeyName);

	Blackboard->SetValue<UBlackboardKeyType_Bool>(CanBeStunnedKey, true);
}

void AGormController::OwnerRevive()
{
	Super::OwnerRevive();
	Blackboard->SetValue<UBlackboardKeyType_Bool>(CanBeStunnedKey, true);
}