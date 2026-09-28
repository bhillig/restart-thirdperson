// Brandon Hillig 2026

#pragma once

#include "NativeGameplayTags.h"

namespace RSGameplayTags
{
	/** Actions */
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Action_Test);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Action_Sprint);

	/** Action Effects */
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(ActionEffect_Sprint);

	/** Status Effects */
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(StatusEffect_Sprinting);

	/** Attributes */
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Health);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_HealthMax);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_MoveSpeed);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_MoveSpeedMultiplier);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Rage);

}
