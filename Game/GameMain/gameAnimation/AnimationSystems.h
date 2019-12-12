#pragma once
#include "ecsSystemIncludes.h"
#include "AnimationComponents.h"
#include "AnimationEvents.h"

namespace ecs {

	namespace systems {

		struct SkeletonSystem : public ECSSystem<SkeletonSystem> {
			SkeletonSystem();
			virtual ~SkeletonSystem();

			UINT animationSelect = 0;

			void updateEntity(FilteredEntity& entity, float delta) override;
		};

		class PingListenerSystem : public ECSSystem<PingListenerSystem>
		{
		public:
			PingListenerSystem()
			{
				updateType = EventReader;
				typeFilter.addRequirement(ecs::events::PingEvent::typeID);
			}
			virtual ~PingListenerSystem() {}
			void readEvent(BaseEvent& event, float delta) override;
		};
	}
}