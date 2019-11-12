#include "AIEcsFunctions.h"
#include "../../AI/includes/GridProp.h"
#include "AIComponents.h"
#include "../gameUtility/UtilityComponents.h"
#include "GridEcsFunctions.h"
#include "../../AI/includes/GridFunctions.h"
#include "../UI/UIComponents.h"
#include "../gameAnimation/AnimationComponents.h"
#include "../MeshContainer/MeshContainer.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include "..//..//Graphics/includes/Window.h"

#include "..//gameGameLoop/GameLoopEvents.h"

namespace AIEcsFunctions
{
	// Creates a army component of each player
	void CreatePlayers(ecs::EntityComponentSystem& rEcs)
	{
		FLOAT client_width = graphics::GetDisplayResolution().x;
		FLOAT client_height = graphics::GetDisplayResolution().y;

		FLOAT text_height = 400;
		FLOAT text_width = 200;
		FLOAT unit_text_height = 75;
		FLOAT unit_text_width = 100;

		FLOAT ui_padding = 50;
		ecs::components::UIDrawColorComponent gay_color_comp;
		ecs::components::UIIWant yaaa;
		gay_color_comp.mColor = brushColors::Gray;

		for (size_t i = 0; i < 4; i++)
		{
			ecs::components::ArmyComponent army;
			ecs::components::UITextComponent command_text_comp;
			command_text_comp.mStrText = L"NO STATE";
			ecs::components::UIDrawPosComponent text_pos_comp;
			ecs::components::UIDrawColorComponent text_color_comp;
			
			
			
			switch (i)
			{
			case 0:
				text_pos_comp.mDrawArea.top = ui_padding;
				text_pos_comp.mDrawArea.bottom = text_height + ui_padding;
				text_pos_comp.mDrawArea.left = 0 + ui_padding;
				text_pos_comp.mDrawArea.right = text_width + ui_padding;

				text_color_comp.mColor = brushColors::Red;
				break;
			case 1:
				text_pos_comp.mDrawArea.top = 0 + ui_padding;
				text_pos_comp.mDrawArea.bottom = text_height + ui_padding;
				text_pos_comp.mDrawArea.left = client_width - text_width - ui_padding;
				text_pos_comp.mDrawArea.right = client_width - ui_padding;

				text_color_comp.mColor = brushColors::Purple;
				break;
			case 2:
				text_pos_comp.mDrawArea.top = client_height - text_height - ui_padding;
				text_pos_comp.mDrawArea.bottom = client_height - ui_padding;
				text_pos_comp.mDrawArea.left = 0 + ui_padding;
				text_pos_comp.mDrawArea.right = text_width + ui_padding;

				text_color_comp.mColor = brushColors::Blue;
				break;
			case 3:
				text_pos_comp.mDrawArea.top = client_height - text_height - ui_padding;
				text_pos_comp.mDrawArea.bottom = client_height - ui_padding;
				text_pos_comp.mDrawArea.left = client_width - text_width - ui_padding;
				text_pos_comp.mDrawArea.right = client_width - ui_padding;

				text_color_comp.mColor = brushColors::Green;
				break;
			default:
				text_pos_comp.mDrawArea.top = 100;
				text_pos_comp.mDrawArea.bottom = 200;
				text_pos_comp.mDrawArea.left = 100;
				text_pos_comp.mDrawArea.right = 200;

				text_color_comp.mColor = brushColors::Black;
				break;
			}



			army.playerID = (PLAYER)i;

			//Create the user entity
			rEcs.createEntity(army, command_text_comp, text_pos_comp, text_color_comp);
			rEcs.createEntity(text_pos_comp, gay_color_comp, yaaa);
		}
	}

	void CreateUnits(ecs::EntityComponentSystem& rEcs)
	{
		//ecs::events::GameStartEvent eve;
		//rEcs.createEvent(eve);
		//	//Clear the army vector before we start creating the next players army.
	}
}