// Brandon Hillig 2026


#include "ActionSystem/RSAttributeSet.h"

URSHealthAttributeSet::URSHealthAttributeSet()
{
	HealthMax = FRSAttribute(100.f);
	Health = FRSAttribute(HealthMax.GetValue());
}
