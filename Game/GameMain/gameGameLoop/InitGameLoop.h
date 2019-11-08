#pragma once
#include "ecs.h"
#include "GameLoopSystems.h"

void InitGameLoop(ecs::EntityComponentSystem& rECS)
{
	// Update ssystems
	rECS.createSystem<ecs::systems::GameLoopSystem>();
	rECS.createSystem<ecs::systems::WaitForStartupSystem>(1);

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
}