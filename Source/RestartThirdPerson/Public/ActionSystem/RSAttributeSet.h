// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "RSAttributeSet.generated.h"

/** 
 *  Attribute
 */
USTRUCT()
struct FRSAttribute
{
	GENERATED_BODY()

	/** Constructors */
	FRSAttribute() {}
	FRSAttribute(float InBaseValue) : BaseValue(InBaseValue) {}

	/** Base value of the attribute */
	UPROPERTY(EditAnywhere, Category="Attributes")
	float BaseValue = 0.0f;

	/** Modifier applied on top of the base value */
	UPROPERTY(Transient)
	float Modifier = 0.0f;

	/** Returns the calculated value of the attribute */
	float GetValue() const
	{
		return BaseValue + Modifier;
	}
};

/**
 *  Base Attribute Set
 */
UCLASS()
class RESTARTTHIRDPERSON_API URSAttributeSet : public UObject
{
	GENERATED_BODY()
};

/** 
 *  Health Attribute Set
 */
UCLASS()
class URSHealthAttributeSet : public URSAttributeSet
{
	GENERATED_BODY()

protected:
	/** Health Attribute */
	UPROPERTY(EditAnywhere, Category="Attributes")
	FRSAttribute Health;

	/** Health Max Attribute */
	UPROPERTY(EditAnywhere, Category = "Attributes")
	FRSAttribute HealthMax;

public:
	/** Constructor */
	URSHealthAttributeSet();
};
