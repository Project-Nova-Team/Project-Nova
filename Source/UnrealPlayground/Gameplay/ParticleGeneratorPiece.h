#pragma once

#include "CoreMinimal.h"
#include "GeneratorPiece.h"
#include "ParticleGeneratorPiece.generated.h"

class UParticleSystemComponent;

UCLASS()
class UNREALPLAYGROUND_API AParticleGeneratorPiece : public AGeneratorPiece
{
	GENERATED_BODY()

public:
	AParticleGeneratorPiece();

	void BreakPiece() override;

protected:
	
	UPROPERTY(Category = Mesh, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* ParticleSystem;
};
