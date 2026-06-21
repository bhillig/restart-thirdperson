// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_FootOnGround.generated.h"

UENUM(BlueprintType)
enum class EFootType : uint8
{
	Left = 0,
	Right = 1
};

/**
 * 
 */
UCLASS()
class RESTARTTHIRDPERSON_API UAnimNotify_FootOnGround : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Foot")
	EFootType FootType;

	// Left Foot
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Left Foot")
	FName LeftFootBoneName = "foot_l";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Left Foot")
	TObjectPtr<USoundBase> LeftFootOnPlasterSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Left Foot")
	TObjectPtr<USoundBase> LeftFootOnGlassSound;

	// Right Foot
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Right Foot")
	FName RightFootBoneName = "foot_r";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Right Foot")
	TObjectPtr<USoundBase> RightFootOnPlasterSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Right Foot")
	TObjectPtr<USoundBase> RightFootOnGlassSound;

protected:

	void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

};
