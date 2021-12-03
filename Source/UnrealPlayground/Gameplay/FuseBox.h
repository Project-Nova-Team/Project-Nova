#pragma once

#include "CoreMinimal.h"
#include "Generator.h"
#include "FuseBox.generated.h"


UCLASS()
class UNREALPLAYGROUND_API AFuseBox : public AGenerator
{
	GENERATED_BODY()

public:
	bool CanInteract() const override { return false; }

	void ReceivePieceRepaired() override;
};
