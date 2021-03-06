#pragma once
#include "ecsComponentIncludes.h"
#include "InputBackend.h"

/*
GUIDE

This shouldnt be used since its te backend that updates the components you should use

it holds all values in its members and a connection to the webBackend

*/


namespace ecs
{
	namespace components
	{
		struct InputBackendComp : public ecs::ECSComponent<InputBackendComp>
		{
			// The input system that holds all input to be set into components
			InputBackend* backend = nullptr;
			int mPlacedTraps[4] = {4, 4, 4, 4};
		};
	}
}

