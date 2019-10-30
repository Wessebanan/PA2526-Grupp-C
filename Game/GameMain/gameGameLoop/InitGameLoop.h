#pragma once
#include "ecs.h"
#include "GameLoopSystems.h"

void InitGameLoop(ecs::EntityComponentSystem& rECS)
{
	// Update ssystems
	rECS.createSystem<ecs::systems::GameLoopSystem>();
	rECS.createSystem<ecs::systems::GameLoopAliveSystem>(1);

	// Event systems
	rECS.createSystem<ecs::systems::GameStartSystem>(0);
	rECS.createSystem<ecs::systems::RoundOverSystem>(2);
	rECS.createSystem<ecs::systems::RoundStartSystem>(3);

	// COMPONENTS
	ecs::components::GameLoopComponent gameloop_comp;
	gameloop_comp.mPlayerPoints[0] = 0;
	gameloop_comp.mPlayerPoints[1] = 0;
	gameloop_comp.mPlayerPoints[2] = 0;
	gameloop_comp.mPlayerPoints[3] = 0;
	ecs::components::GameSettingsComponent gameloop_settings_comp;
	gameloop_settings_comp.mMapSettings = 1; // Holme = 1

	ecs::components::UITextComponent time_text_comp;
	ecs::components::UIDrawPosComponent text_pos_comp;
	text_pos_comp.mDrawArea.top = 450;
	text_pos_comp.mDrawArea.bottom = 550;
	text_pos_comp.mDrawArea.left = 200;
	text_pos_comp.mDrawArea.right = 800;
	ecs::components::UIDrawColorComponent text_color_comp;
	text_color_comp.mColor = brushColors::White;

	rECS.createEntity(
		gameloop_comp, 
		gameloop_settings_comp, 
		time_text_comp, 
		text_pos_comp, 
		text_color_comp
	);

	// Player points

	FLOAT client_width = graphics::GetDisplayResolution().x;
	FLOAT client_height = graphics::GetDisplayResolution().y;

	FLOAT text_height = 100;
	FLOAT text_width = 200;
	FLOAT unit_text_height = 75;
	FLOAT unit_text_width = 100;

	FLOAT ui_padding = 50;

	ecs::components::GamePointsComponent point_comp;
	ecs::components::UITextComponent player_text;
	player_text.mStrText = "Not started";
	ecs::components::UIDrawPosComponent player_text_pos_comp;
	ecs::components::UIDrawColorComponent player_text_color_comp;

	for (size_t i = 0; i < 4; i++)
	{


		switch (i)
		{
		case 0:
			player_text_pos_comp.mDrawArea.top = ui_padding + 50;
			player_text_pos_comp.mDrawArea.bottom = text_height + ui_padding;
			player_text_pos_comp.mDrawArea.left = 0 + ui_padding;
			player_text_pos_comp.mDrawArea.right = text_width + ui_padding;

			player_text_color_comp.mColor = brushColors::Red;
			break;
		case 1:
			player_text_pos_comp.mDrawArea.top = 0 + ui_padding + 50;
			player_text_pos_comp.mDrawArea.bottom = text_height + ui_padding;
			player_text_pos_comp.mDrawArea.left = client_width - text_width - ui_padding;
			player_text_pos_comp.mDrawArea.right = client_width - ui_padding;
			
			player_text_color_comp.mColor = brushColors::Gray;
			break;
		case 2:
			player_text_pos_comp.mDrawArea.top = client_height - text_height - ui_padding - 50;
			player_text_pos_comp.mDrawArea.bottom = client_height - ui_padding;
			player_text_pos_comp.mDrawArea.left = 0 + ui_padding;
			player_text_pos_comp.mDrawArea.right = text_width + ui_padding;
			
			player_text_color_comp.mColor = brushColors::Cyan;
			break;
		case 3:
			player_text_pos_comp.mDrawArea.top = client_height - text_height - ui_padding - 50;
			player_text_pos_comp.mDrawArea.bottom = client_height - ui_padding;
			player_text_pos_comp.mDrawArea.left = client_width - text_width - ui_padding;
			player_text_pos_comp.mDrawArea.right = client_width - ui_padding;
			
			player_text_color_comp.mColor = brushColors::Purple;
			break;
		default:
			player_text_pos_comp.mDrawArea.top = 100;
			player_text_pos_comp.mDrawArea.bottom = 200;
			player_text_pos_comp.mDrawArea.left = 100;
			player_text_pos_comp.mDrawArea.right = 200;
			
			player_text_color_comp.mColor = brushColors::Black;
			break;
		}


		//Create the user entity
		rECS.createEntity(point_comp, player_text, player_text_pos_comp, player_text_color_comp);
	}
}