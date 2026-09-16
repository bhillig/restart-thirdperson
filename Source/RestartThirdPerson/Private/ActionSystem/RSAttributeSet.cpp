// Brandon Hillig 2026


#include "ActionSystem/RSAttributeSet.h"

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

