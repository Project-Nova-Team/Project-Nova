#pragma once

#include "CoreMinimal.h"
#include "Trigger.h"
#include "AudioTrigger.generated.h"

class UAudioComponent;

UCLASS()
class UNREALPLAYGROUND_API AAudioTrigger : public ATrigger
{
	GENERATED_BODY()
	
public:	
	AAudioTrigger();

protected:

	/** Component that plays the bark*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Volume")
	UAudioComponent* AudioComponent;

	virtual void ExecuteTrigger(APawn* Sender) override;
};
