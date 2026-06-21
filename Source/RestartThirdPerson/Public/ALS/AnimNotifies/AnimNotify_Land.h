// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_Land.generated.h"

/**
 * 
 */
UCLASS()
class RESTARTTHIRDPERSON_API UAnimNotify_Land : public UAnimNotify
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Land")
	TObjectPtr<USoundBase> PlasterLandSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Land")
	TObjectPtr<USoundBase> GlassLandSound;

protected:

	void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

};
