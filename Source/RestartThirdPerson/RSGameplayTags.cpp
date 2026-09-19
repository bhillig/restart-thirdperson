// Brandon Hillig 2026


#include "RSGameplayTags.h"

namespace RSGameplayTags
{
	/** Actions */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Action_Test, "Action.Test", "Tag to identify the Test Action");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Action_Sprint, "Action.Sprint", "Tag to identify the Sprint Action");

	/** Attributes */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Health, "Attribute.Health", "Health Attribute");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_HealthMax, "Attribute.HealthMax", "Health Max Attribute");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_MoveSpeed, "Attribute.MoveSpeed", "Move Speed Attribute");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Rage, "Attribute.Rage", "Rage Attribute");
}
