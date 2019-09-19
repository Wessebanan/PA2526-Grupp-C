#include "pch.h"
#include "..//InputHandler/includes/InitInputHandler.h"

// Crashes when tests try to make multiple inputSystems so this is how you init the input Backend
InputSystem* gInputSystem;
TEST(ECS, InitInput) 
{
	gInputSystem = new InputSystem();
}

TEST(ECS, InitCompEntSys) 
{
	ecs::EntityComponentSystem rECS;
	
	initInputECS(rECS, gInputSystem);
	
	rECS.update(0.3f);
}