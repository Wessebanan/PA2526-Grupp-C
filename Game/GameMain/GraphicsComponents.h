#pragma once
#include "ecs.h"
#define COMP(name) struct name : public ecs::ECSComponent<name>


namespace ecs
{
	namespace components
	{
		COMP(ModelComponent)
		{
			int index;
		}


	} // components
} // ecs

