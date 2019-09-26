#pragma once
#include "ecsSystemIncludes.h"
#include "InputComponents.h"
#include "InterpretWebEvents.h"

namespace ecs
{
	namespace systems
	{
		class ChangeFSMSystem : public ecs::ECSSystem<ChangeFSMSystem>
		{
		public:
			ChangeFSMSystem();
			~ChangeFSMSystem();
			void updateEntity(FilteredEntity& _entityInfo, float _delta) override;
		private:
			STATE mCurrStates[4];
		};
	}
}