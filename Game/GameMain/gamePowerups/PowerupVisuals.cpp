#include "PowerupVisuals.h"
#include "PowerupComponents.h"
#include "../gameUtility/UtilityComponents.h"

namespace ecs
{
	namespace systems
	{
		PowerupLootAnimator::PowerupLootAnimator()
		{
			updateType = EntityUpdate;
			typeFilter.addRequirement(components::PowerupLootComponent::typeID);
			typeFilter.addRequirement(components::TransformComponent::typeID);
		}

		PowerupLootAnimator::~PowerupLootAnimator()
		{
			//
		}

		void PowerupLootAnimator::updateEntity(FilteredEntity& _entityInfo, float _delta)
		{
			components::TransformComponent* p_transform_comp = _entityInfo.getComponent<components::TransformComponent>();
			p_transform_comp->rotation.y += 0.5f * _delta;
		}
	}
}