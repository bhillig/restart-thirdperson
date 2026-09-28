// Brandon Hillig 2026


#include "RSGameplayTags.h"

namespace RSGameplayTags
{
	/** Actions */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Action_Test, "Action.Test", "Tag to identify the Test Action");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Action_Sprint, "Action.Sprint", "Tag to identify the Sprint Action");

	/** Action Effects */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(ActionEffect_Sprint, "ActionEffect.Sprint", "Tag to identify the Sprint Action Effect");

	/** Status Effects */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(StatusEffect_Sprinting, "StatusEffect.Sprinting", "Tag to identify the Sprinting Status Effect");

	/** Attributes */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Health, "Attribute.Health", "Health Attribute");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_HealthMax, "Attribute.HealthMax", "Health Max Attribute");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_MoveSpeed, "Attribute.MoveSpeed", "Move Speed Attribute");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_MoveSpeedMultiplier, "Attribute.MoveSpeedMultiplier", "Move Speed Multiplier Attribute");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Rage, "Attribute.Rage", "Rage Attribute");
}
