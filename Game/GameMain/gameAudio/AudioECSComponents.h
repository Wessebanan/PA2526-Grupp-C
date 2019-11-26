#pragma once

#include "ecsComponentIncludes.h"

namespace ecs
{
	namespace components
	{
		struct SoundCooldownComponent : public ECSComponent<SoundCooldownComponent>
		{
			float timeElapsed;
			AudioName audioName;
		};
	}
}