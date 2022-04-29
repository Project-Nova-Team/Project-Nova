#include "ObjectiveWidget.h"
#include "ObjectiveWidget.h"
#include "Components/TextBlock.h"

void UObjectiveWidget::UpdateObjectiveWidget(FText& ObjectiveText)
{
	TextBlock->SetText(ObjectiveText);
	PlayAnimation(Animation);
}

void UObjectiveWidget::Clear()
{
	PlayAnimation(ClearAnimation);
}