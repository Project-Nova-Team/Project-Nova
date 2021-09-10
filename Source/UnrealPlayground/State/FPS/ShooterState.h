#pragma once

#include "CoreMinimal.h"
#include "../State.h"
#include "../../Player/Shooter.h"
#include "../../ShooterGameMode.h"
#include "../../Utility/DelayedActionManager.h"
#include "ShooterState.generated.h"

UCLASS()
class UNREALPLAYGROUND_API UShooterState : public UState
{
	GENERATED_BODY()
	
public:
	UShooterState() { }
	~UShooterState() { }

	virtual void Initialize(UStateMachine* StateMachine, UObject* ContextObject) override;
	virtual void OnEnter() override { }
	virtual void OnExit() override { }
	virtual void Tick(const float DeltaTime) override { Super::Tick(DeltaTime); }

protected:
	/** The context object as a shooter this state is going to perform game logic on*/
	AShooter* Shooter;

	/** Shorthand for getting the DelayedActionManager*/
	UDelayedActionManager* GetDelayedActionManager() const 
	{
		return Shooter->GetWorld()->GetAuthGameMode<AShooterGameMode>()->GetDelayedActionManager();
	}
};
