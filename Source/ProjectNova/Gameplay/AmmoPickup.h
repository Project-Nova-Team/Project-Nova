#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractiveObject.h"
#include "../Weapon/Gun.h"
#include "AmmoPickup.generated.h"

USTRUCT(BlueprintType)
struct FAmmoPackage
{
	GENERATED_BODY()

public:

	/** Class of gun this ammo is used for*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo Pickup")
	TSubclassOf<class AGun> GunClass;

	/** Amount of ammo provided by this pickup*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo Pickup")
	int32 AmmoAmount;

	bool operator==(const FAmmoPackage& rhs)
	{
		return GunClass.Get() == rhs.GunClass.Get() && AmmoAmount == rhs.AmmoAmount;
	}

	friend bool operator==(const FAmmoPackage& lhs, const FAmmoPackage& rhs) 
	{ 
		return lhs.GunClass.Get() == rhs.GunClass.Get() && lhs.AmmoAmount == rhs.AmmoAmount; 
	}
};

UCLASS()
class PROJECTNOVA_API AAmmoPickup : public AActor, public IInteractiveObject
{
	GENERATED_BODY()
	
public:	

	// Sets default values for this actor's properties
	AAmmoPickup();

	/** Ammount of ammo and type of gun this is used for*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo Pickup | Ammo")
	FAmmoPackage Package;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh", meta =(AllowPrivateAccess="true"))
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo Pickup | Interaction")
	FInteractionPrompt Prompt;

protected:

	virtual void InteractionEvent(APawn* EventSender) override;

	FInteractionPrompt& GetInteractionPrompt() override { return Prompt; }
};
