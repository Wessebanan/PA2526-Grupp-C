#pragma once
#include "ecsSystemIncludes.h"
#include "SoundECSEvents.h"

namespace ecs
{
	namespace systems
	{
		class SoundMessageSystem : public ECSSystem<SoundMessageSystem>
		{
		public:
			SoundMessageSystem();
			virtual ~SoundMessageSystem();

			void readEvent(BaseEvent& rEvent, float delta) override;
		};
	}
}