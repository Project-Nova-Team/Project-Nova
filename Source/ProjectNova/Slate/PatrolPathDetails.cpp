#include "PatrolPathDetails.h"

#if WITH_EDITOR

#include "Editor/PropertyEditor/Public/DetailLayoutBuilder.h"
#include "Editor/PropertyEditor/Public/DetailCategoryBuilder.h"
#include "Editor/PropertyEditor/Public/DetailWidgetRow.h"
#include "Runtime/Slate/Public/Widgets/Text/STextBlock.h"
#include "Runtime/Slate/Public/Widgets/Input/SButton.h"

#define LOCTEXT_NAMESPACE "PatrolPathDetails"

TSharedRef<IDetailCustomization> FPatrolPathDetails::MakeInstance()
{
	return MakeShareable(new FPatrolPathDetails);
}

void FPatrolPathDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	/*
	IDetailCategoryBuilder& Category = DetailBuilder.EditCategory("Patrol Path", FText::GetEmpty(), ECategoryPriority::Important);
	APatrolPath* DetailsActor = nullptr;
	TArray<TWeakObjectPtr<UObject>> CustomizedObjects;
	DetailBuilder.GetObjectsBeingCustomized(CustomizedObjects);

	for (TWeakObjectPtr<UObject> Object : CustomizedObjects)
	{
		if (Object.IsValid())
		{
			DetailsActor = Cast<APatrolPath>(Object);

			if (DetailsActor != nullptr) 
			{
				break;
			}
		}
	}

	check(DetailsActor);

	Category.AddCustomRow(LOCTEXT("RowSearchName", "Add Point"))
		.NameContent()
		[
			SNew(STextBlock)
			.Text(LOCTEXT("DetailName", "Add Point"))
			.Font(IDetailLayoutBuilder::GetDetailFont())
		]
		.ValueContent()
		[
			SNew(SButton)
			.Text(LOCTEXT("ButtonText", "Add Point"))
			.HAlign(HAlign_Center)
			.ToolTipText(LOCTEXT("ButtonTooltip", "Adds a new point to the path"))
			.OnClicked_UObject(DetailsActor, &APatrolPath::AddPoint)
		];
		*/
}

#undef LOCTEXT_NAMESCAPE

#endif
