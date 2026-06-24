// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WorldSpaceUserWidget.generated.h"

class USizeBox;
/**
 * 
 */
UCLASS()
class RESTARTTHIRDPERSON_API UWorldSpaceUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> ParentSizeBox;

	UPROPERTY(BlueprintReadWrite, meta=(ExposeOnSpawn = "true"))
	TObjectPtr<AActor> AttachedActor;

	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
	FVector WorldOffset;
};
