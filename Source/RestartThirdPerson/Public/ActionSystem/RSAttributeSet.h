// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "RSAttributeSet.generated.h"

class URSActionSystemComponent;
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
	/** Called when the game begins */
	virtual void Initialize() {}

	/** Called when an attribute on this set changes */
	virtual void PostAttributeChange() {}

protected:
	/** Returns the owning action system component */
	URSActionSystemComponent* GetOwningComponent() const;
};

/** 
 *  Health Attribute Set
 */
UCLASS()
class URSHealthAttributeSet final : public URSAttributeSet
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

/**
 *  Character Attribute Set
 */
UCLASS()
class URSCharacterAttributeSet final : public URSAttributeSet
{
	GENERATED_BODY()

protected:
	/** Move Speed Attribute */
	UPROPERTY(EditAnywhere, Category = "Attributes")
	FRSAttribute MoveSpeed;

	/** Move Speed Multiplier Attribute */
	UPROPERTY(EditAnywhere, Category = "Attributes")
	FRSAttribute MoveSpeedMultiplier;

	/** Applies the move speed on the character movement component */
	void ApplyMoveSpeed();

public:
	/** URSAttributeSet */
	virtual void Initialize() override;

	virtual void PostAttributeChange() override;
	/** URSAttributeSet */

	/** Constructor */
	URSCharacterAttributeSet();
};

/**
 *  Rage Attribute Set
 */
UCLASS()
class URSRageAttributeSet final : public URSAttributeSet
{
	GENERATED_BODY()

protected:
	/** Rage Attribute */
	UPROPERTY(EditAnywhere, Category = "Attributes")
	FRSAttribute Rage;

public:
	/** Constructor */
	URSRageAttributeSet();
};
