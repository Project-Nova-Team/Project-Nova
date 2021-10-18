#pragma once

#include "CoreMinimal.h"
#include "Gun.h"
#include "Shotgun.generated.h"


UCLASS()
class UNREALPLAYGROUND_API AShotgun : public AGun
{
	GENERATED_BODY()

public:
	AShotgun();

protected:

	void FireStraight() override;
	void FireWithNoise() override;

	/** How many bullets does each shotgun round fire*/
	UPROPERTY(EditAnywhere, Category = "Weapon | Firing")
	int32 BulletsPerShot;
};
