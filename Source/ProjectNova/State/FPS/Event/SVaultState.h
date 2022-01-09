#pragma once

#include "CoreMinimal.h"
#include "SEventState.h"
#include "Components/SkeletalMeshComponent.h"
#include "SVaultState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAnimVault);

UCLASS()
class PROJECTNOVA_API USVaultState : public USEventState
{
	GENERATED_BODY()

public:
	USVaultState() {}
	~USVaultState() {}

	void OnEnter() override; 
	void OnExit() override;

	void ReceiveVaultAnimComplete();

	FAnimVault OnVaultEnter;
};
