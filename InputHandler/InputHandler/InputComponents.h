#pragma once
#include "ecs.h"
#include "InputSystems.h"


namespace ecs 
{
	namespace components
	{
		struct InputBackendComp : public ecs::ECSComponent<InputBackendComp>
		{
			// The input system that holds all input to be set into components
			InputSystem* backend;
		};


		struct KeyComp : public ecs::ECSComponent<KeyComp>
		{
			int key = -1;
			bool pressed = false;
		};

		struct KeyboardComponent : public ecs::ECSComponent<KeyboardComponent>
		{
			// Movement
			KeyComp W;
			KeyComp S;
			KeyComp A;
			KeyComp D;

			// Reset
			KeyComp R;
			// Exit or Menu
			KeyComp ECS;

			// Extra for debug
			KeyComp Q;
			KeyComp E;
		};
	}
}

