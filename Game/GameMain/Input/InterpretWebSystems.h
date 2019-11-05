#pragma once
#include "ecsSystemIncludes.h"
#include "../Input/InitInputComponents.h"
#include "../Input/InterpretWebEvents.h"

namespace ecs
{
	namespace systems
	{
		// Looks thro the commands and creates events when a diffrent command comes in
		class ChangeFSMSystem : public ecs::ECSSystem<ChangeFSMSystem>
		{
		public:
			ChangeFSMSystem();
			~ChangeFSMSystem();
			void updateEntity(FilteredEntity& _entityInfo, float _delta) override;
		};

		// Looks if a new trap was sent and then creates a event for that
		class TrapEventSystem : public ecs::ECSSystem<TrapEventSystem>
		{
		public:
			TrapEventSystem();
			~TrapEventSystem();
			void updateEntity(FilteredEntity& _entityInfo, float _delta) override;
		};
	}
}