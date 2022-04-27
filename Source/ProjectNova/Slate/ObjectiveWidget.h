#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ObjectiveWidget.generated.h"

UCLASS()
class PROJECTNOVA_API UObjectiveWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	/** Animation played upon receiving objective update*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWidgetAnimation* Animation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* TextBlock;

public:

	void UpdateObjectiveWidget(FText& ObjectiveText);
};
