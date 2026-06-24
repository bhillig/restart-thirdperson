// Brandon Hillig 2026


#include "Widgets/WorldSpaceUserWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/SizeBox.h"


void UWorldSpaceUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!AttachedActor || !ParentSizeBox)
	{
		return;
	}

	FVector2D ScreenPosition;
	if (UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(GetOwningPlayer(), AttachedActor->GetActorLocation() + WorldOffset, ScreenPosition, false))
	{
		ParentSizeBox->SetRenderTranslation(ScreenPosition);
	}
}
