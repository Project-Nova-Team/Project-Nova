#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractiveObject.h"
#include "Removable.h"
#include "../Weapon/Gun.h"
#include "AmmoPickup.generated.h"

UCLASS()
class UNREALPLAYGROUND_API AAmmoPickup : public AActor, public IInteractiveObject, public IRemovable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAmmoPickup();

	virtual void InteractionEvent(APawn* EventSender) override;

protected:

	/** How much ammo does the player get when getting this pickup*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AmmoAmount;

	/** What type of ammo does this pickup give*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EGunClass> GunType;

	UPROPERTY(VisibleAnywhere, Category = "Mesh", meta =(AllowPrivateAccess="true"))
	UStaticMeshComponent* Mesh;
};
