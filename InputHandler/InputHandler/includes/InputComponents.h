#pragma once
#include "ecsComponentIncludes.h"
#include <DirectXMath.h>

/*
GUIDE

To use this you get the components and the components hold diffrent data,

* Keyboard holds keys from the keyboard that you want to see in BOOLs

* Mouse hold the delta values that has been calculated between frames in a float2 (diffFloat2)
-- the length of this is held in a float (diffLegth)
- Also holds the left and rigth buttons as BOOLs
- also holds the vurrent position to be used in menus

* Users holds the diffrent button, divided into diffrent button-sets
-- the index is the player

*/

/// vvvvv THESE ARE PASTED FROM INPUTBACKEND.H vvvvv
// The tile the user has selected
struct WebTileCopy
{
	// -1 if none selected
	int mCordX = -1;
	// -1 if none selected
	int mCordY = -1;
};
// The last button pressed
struct WebButtonCopy
{
	// -1 if none selected yet
	int mButton = -1;
};
// The last command selected by the user
struct WebCommandCopy
{
	// holds the command, is NaN if none has been selected yet
	std::string mCommand = "NaN";
};
/// ^^^^^ THESE ARE PASTED FROM INPUTBACKEND.H  ^^^^

namespace ecs 
{
	namespace components
	{
		// a set of bools that are true if thier key is pressed
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

		// Holds mousebuttons, current position and the movment from last frame (diff...)
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

		// the 0-3 buttons in prep phase for the users
		struct UserButtonComponent : public ecs::ECSComponent<UserButtonComponent>
		{
			// Index is the user
			WebButtonCopy userButtons[4];
		};
		// the (0,0)-(3,3) tiles in prep phase for the users
		struct UserTileComponent : public ecs::ECSComponent<UserTileComponent>
		{
			// Index is the user
			WebTileCopy userTiles[4];
		};
		// NOT IMPLEMENTED
		struct UserNameComponent : public ecs::ECSComponent<UserNameComponent>
		{
			// Index is the user
			std::string names[4];
		};
		// The commands in the battle phase for the users
		struct UserCommandComponent : public ecs::ECSComponent<UserCommandComponent>
		{
			// Index is the user
			WebCommandCopy userCommands[4];
		};
	}
}

