#pragma once
#include "ecs.h"
#include "HandleInputBackend.h"

// Initiates ECS for input, IMPORTANT: pInputBackend needs to be initalized ONCE beforehand
void initInputECS(ecs::EntityComponentSystem& rECS, InputBackend* pInputBackend);

void initInputECS(ecs::EntityComponentSystem& rECS, InputBackend* pInputBackend)
{
	//// SYSTEMS
	TypeID var = ecs::systems::HandleInputBackend::typeID;
	ecs::systems::HandleInputBackend* psys = rECS.createSystem<ecs::systems::HandleInputBackend>(0);// parameter är layer

	rECS.createSystem<ecs::systems::HandleKeyboardSystem>(1);
	rECS.createSystem<ecs::systems::HandleMouseSystem>(1);
	rECS.createSystem<ecs::systems::HandleWebSystem>(1);

	//// COMPONENTS

	// Backend Components
	ecs::components::InputBackendComp backend = ecs::components::InputBackendComp();
	backend.backend = pInputBackend;
	//backend.backend = new InputBackend();

	// Keyboard Components
	ecs::components::KeyboardComponent kbComp = ecs::components::KeyboardComponent();

	// Mouse Components
	ecs::components::MouseComponent mouseComp = ecs::components::MouseComponent();

	// Web Components
	ecs::components::UserButtonComponent userButtonComp = ecs::components::UserButtonComponent();
	ecs::components::UserTileComponent userTileComp = ecs::components::UserTileComponent();
	ecs::components::UserCommandComponent userCommandComp = ecs::components::UserCommandComponent();
	//ecs::components::UserNameComponent userNameComp = ecs::components::UserNameComponent();

	ecs::BaseComponent* components[] = 
	{ 
		&userButtonComp, 
		&userTileComp,
		&userCommandComp,
		&mouseComp,
		&kbComp,
		&backend
	};

	ecs::ComponentList list;

	list.initialInfo = components;
	list.componentCount = 6;

	//// ENTITIES
	rECS.createEntity(
		list
		);


}
