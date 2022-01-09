#pragma once

#include "CoreMinimal.h"
#include "../State.h"
#include "../../Player/Shooter.h"
#include "../../ShooterGameMode.h"
#include "../../Utility/DelayedActionManager.h"
#include "ShooterState.generated.h"

UCLASS()
class PROJECTNOVA_API UShooterState : public UState
{
	GENERATED_BODY()
	
public:
	UShooterState() { }
	~UShooterState() { }

	virtual void Initialize(UStateMachine* StateMachine, UObject* ContextObject) override;

protected:
	/** The context object as a shooter this state is going to perform game logic on*/
	AShooter* Shooter;

	/** Shorthand for getting the DelayedActionManager*/
	UDelayedActionManager* GetDelayedActionManager() const;
};
