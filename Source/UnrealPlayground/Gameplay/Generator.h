#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractiveObject.h"
#include "Generator.generated.h"


UCLASS()
class UNREALPLAYGROUND_API AGenerator : public AActor, public IInteractiveObject
{
	GENERATED_BODY()
	
public:	
	AGenerator();

	void InteractionEvent(APawn* EventSender) override;

protected:

	UPROPERTY(VisibleANywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;
};
