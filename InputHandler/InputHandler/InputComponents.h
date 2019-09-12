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

		struct KeyboardComponent : public ecs::ECSComponent<KeyboardComponent>
		{
			// Movement
			bool W = false;
			bool S = false;
			bool A = false;
			bool D = false;

			// Reset
			bool R = false;
			// Exit or Menu
			bool ECS = false;

			// Extra for debug
			bool Q = false;
			bool E = false;
		};

		
		struct MouseComponent : public ecs::ECSComponent<MouseComponent>
		{
			// Keys
			bool LMB = false;
			bool RMB = false;

			// Directional varibles
			DirectX::XMFLOAT2 diffFloat2;
			float diffLength = 0.0f;

			// Position of the cursor on the whole screen
			DirectX::XMFLOAT2 pos;
		};


		struct WebComponent : public ecs::ECSComponent<KeyboardComponent>
		{
			// Movement
			bool W = false;
			bool S = false;
			bool A = false;
			bool D = false;

			// Reset
			bool R = false;
			// Exit or Menu
			bool ECS = false;

			// Extra for debug
			bool Q = false;
			bool E = false;
		};
	}
}

