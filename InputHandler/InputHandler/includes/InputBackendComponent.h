#pragma once
#include "ecs.h"
#include "InputBackend.h"


namespace ecs
{
	namespace components
	{
		struct InputBackendComp : public ecs::ECSComponent<InputBackendComp>
		{
			// The input system that holds all input to be set into components
			InputBackend* backend = nullptr;
		};
	}
}

