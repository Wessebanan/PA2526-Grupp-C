#pragma once

#include "ecsSystemIncludes.h"
#include "UIComponents.h"
#include "UIEvents.h"
#include "Direct2D.h"

namespace ecs
{
	namespace systems
	{

		class CustomSystem : public ECSSystem<CustomSystem>
		{
		public:
			CustomSystem()
			{
				// Set UpdateType (see SystemUpdateType enum)
				updateType = SystemUpdateType::EventReader;

				// If entity update
				componentFilter.addRequirement(components::HealthComponent::typeID);
				componentFilter.addRequirement(components::PoisonComponent::typeID);

				// If event reader
				eventFilter.addRequirement(events::CreateEntityEvent::typeID);

				// Optional for all systems (try to avoid)
				subscribeEventCreation(events::CreateComponentEvent::typeID);
			}
			virtual ~CustomSystem() {}

			// Override ONE of these
			void updateEntity(FilteredEntity& _entityInfo, float _delta) override;
			void updateMultipleEntities(EntityIterator& _entities, float _delta) override;
			void readEvent(BaseEvent& _event, float _delta) override;
			void act(float _delta) override;
		};
	}
}