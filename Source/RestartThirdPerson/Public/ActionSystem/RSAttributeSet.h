// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "RSAttributeSet.generated.h"

/** 
 *  Attribute
 */
USTRUCT(Blueprintable)
struct FRSAttribute
{
	GENERATED_BODY()

	/** Constructors */
	FRSAttribute() {}
	FRSAttribute(float InBaseValue) : Base(InBaseValue) {}

	/** Base value of the attribute */
	UPROPERTY(EditAnywhere, Category="Attributes")
	float Base = 0.0f;

	/** Modifier applied on top of the base value */
	UPROPERTY(Transient)
	float Modifier = 0.0f;

	/** Returns the calculated value of the attribute */
	float GetValue() const
	{
		return Base + Modifier;
	}
};

/**
 *  Base Attribute Set
 */
UCLASS()
class RESTARTTHIRDPERSON_API URSAttributeSet : public UObject
{
	GENERATED_BODY()

public:
	/** Called when an attribute on this set changes */
	virtual void PostAttributeChange() {}
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
	/** URSAttributeSet */
	virtual void PostAttributeChange() override;
	/** URSAttributeSet */

	/** Constructor */
	URSHealthAttributeSet();
};
