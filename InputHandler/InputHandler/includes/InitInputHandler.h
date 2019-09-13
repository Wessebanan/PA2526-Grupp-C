#pragma once

#ifndef INITINPUTHANDLER_H
#define INITINPUTHANDLER_H


#include "HandleInputSystem.h"

// Initiates the systems and entitys for the InputHandling
void initInputECS(ecs::EntityComponentSystem& rECS);

void initInputECS(ecs::EntityComponentSystem& rECS)
{
	//// SYSTEMS
	rECS.createSystem<ecs::systems::HandleInputSystem>(0);// parameter är layer

	rECS.createSystem<ecs::systems::HandleKeyboardSystem>(1);
	rECS.createSystem<ecs::systems::HandleMouseSystem>(1);
	rECS.createSystem<ecs::systems::HandleWebSystem>(1);

	//// COMPONENTS

	// Backend Components
	ecs::components::InputBackendComp backend = ecs::components::InputBackendComp();
	backend.backend = new InputSystem();

	// Keyboard Components
	ecs::components::KeyboardComponent kbComp = ecs::components::KeyboardComponent();

	// Mouse Components
	ecs::components::MouseComponent mouseComp = ecs::components::MouseComponent();

	// Web Components
	ecs::components::UserButtonComponent userButtonComp = ecs::components::UserButtonComponent();
	ecs::components::UserTileComponent userTileComp = ecs::components::UserTileComponent();
	//ecs::components::UserNameComponent userNameComp = ecs::components::UserNameComponent();


	//// ENTITIES
	rECS.createEntity(
		userButtonComp,
		userTileComp,
		//userNameComp, 
		mouseComp,
		kbComp,
		backend);
}


#endif // !INITINPUTHANDLER_H
