#pragma once
#include "ecsComponentIncludes.h"

namespace ecs 
{
	namespace components
	{
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


		struct UserButtonComponent : public ecs::ECSComponent<UserButtonComponent>
		{
			// [player][button]
			bool buttons[4][4];
		};
		struct UserTileComponent : public ecs::ECSComponent<UserTileComponent>
		{
			// The selected tile cord
			int tile[4][2];
			//int ;
		};
		struct UserNameComponent : public ecs::ECSComponent<UserNameComponent>
		{
			// Name
			std::string names[4];
		};
	}
}

