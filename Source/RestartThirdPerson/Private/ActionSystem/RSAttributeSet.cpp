// Brandon Hillig 2026


#include "ActionSystem/RSAttributeSet.h"

#include "ActionSystem/RSActionSystemComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

URSHealthAttributeSet::URSHealthAttributeSet()
{
	HealthMax = FRSAttribute(100.f);
	Health = FRSAttribute(HealthMax.GetValue());
}

void URSHealthAttributeSet::PostAttributeChange()
{
	Super::PostAttributeChange();

	Health.Base = FMath::Clamp(Health.GetValue(), 0.0f, HealthMax.GetValue());
}

URSCharacterAttributeSet::URSCharacterAttributeSet()
{
	MoveSpeed = FRSAttribute(425.f);
	MoveSpeedMultiplier = FRSAttribute(1.f);
}

void URSCharacterAttributeSet::Initialize()
{
	Super::Initialize();

	ApplyMoveSpeed();
}

void URSCharacterAttributeSet::PostAttributeChange()
{
	Super::PostAttributeChange();

	ApplyMoveSpeed();
}

void URSCharacterAttributeSet::ApplyMoveSpeed()
{
	URSActionSystemComponent* ActionSystemComponent = GetOwningComponent();
	ensure(ActionSystemComponent);

	ACharacter* OwningCharacter = CastChecked<ACharacter>(ActionSystemComponent->GetOwner());
	OwningCharacter->GetCharacterMovement()->MaxWalkSpeed = MoveSpeed.GetValue() * MoveSpeedMultiplier.GetValue();
}

URSRageAttributeSet::URSRageAttributeSet()
{
	Rage = FRSAttribute(0.f);
}

URSActionSystemComponent* URSAttributeSet::GetOwningComponent() const
{
	return Cast<URSActionSystemComponent>(GetOuter());
}