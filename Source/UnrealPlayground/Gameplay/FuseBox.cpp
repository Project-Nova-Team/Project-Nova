#include "FuseBox.h"

//Since the fuse box just instantly fixes itself we call interact to remain consistent with other generators
void AFuseBox::ReceivePieceRepaired()
{
	Execute_BlueprintInteract(this, nullptr);
}
