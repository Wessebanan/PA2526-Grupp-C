#pragma once

#include "ecsComponentIncludes.h"

namespace ecs
{
	namespace components
	{
		struct OceanTileComponent : public ECSComponent<OceanTileComponent>
		{

		};

		struct IsletComponent : public ECSComponent<IsletComponent>
		{
			ID playerId;
		};
	}
}