#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Player/WeaponInput.h"
#include "AIBase.generated.h"

class UInstructionComponent;
class UCombatComponent;
class UHealthComponent;

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

	UPROPERTY(Category = Mesh, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(Category = Combat, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* TraceOrigin;

	UPROPERTY(Category = Instruction, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UInstructionComponent* Instruction;

	UPROPERTY(Category = Combat, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCombatComponent* Combat;

	UPROPERTY(Category = Health, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UHealthComponent* Health;
};
