#include "SpawnAITrigger.h"
#include "../AI/AIGruntController.h"
#include "../AI/AIBase.h"

void ASpawnAITrigger::ExecuteTrigger(APawn* Sender)
{
	Super::ExecuteTrigger(Sender);

	for (AAIBase* AI : SpawnCollection)
	{
		if (AI == nullptr) continue;
		
		AAIBaseController* Controller = Cast<AAIBaseController>(AI->GetController());

		if (Controller == nullptr) continue;

		Controller->SetAIActive(true, bRespawnAtCurrentLocation);
	}
}