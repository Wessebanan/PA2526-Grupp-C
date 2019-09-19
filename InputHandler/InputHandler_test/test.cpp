#include "pch.h"
#include "..//InputHandler/includes/InitInputHandler.h"

// Crashes when tests try to make multiple InputBackends so this is how you init the input Backend
InputBackend* gInputBackend;
TEST(ECS, InitInput) 
{
	gInputBackend = new InputBackend();
}

TEST(ECS, InitCompEntSys) 
{
	ecs::EntityComponentSystem rECS;
	
	initInputECS(rECS, gInputBackend);
	
	rECS.update(0.3f);
}