#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Player/WeaponInput.h"
#include "AIBase.generated.h"

class UInstructionComponent;

UCLASS()
class UNREALPLAYGROUND_API AAIBase : public ACharacter
{
	GENERATED_BODY()

public:
	AAIBase();

	virtual void Tick(float DeltaTime) override;

	UInstructionComponent* GetInstruction() const { return Instruction; }

protected:
	virtual void BeginPlay() override;
	
	FWeaponInput Input;

	UPROPERTY(Category = Instruction, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UInstructionComponent* Instruction;	
};
