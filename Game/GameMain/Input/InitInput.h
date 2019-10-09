#pragma once
#include "InitInputSystems.h"

#include "ecs.h"

void initInput(ecs::EntityComponentSystem& rECS)
{
	rECS.reserveComponentCount<InputBackendComp>(1);
	rECS.reserveComponentCount<KeyboardComponent>(1);
	rECS.reserveComponentCount<MouseComponent>(1);
	rECS.reserveComponentCount<UserButtonComponent>(1);
	rECS.reserveComponentCount<UserTileComponent>(1);
	rECS.reserveComponentCount<UserCommandComponent>(1);

	InputBackend* inp = new InputBackend();

	//// SYSTEMS
	TypeID var = ecs::systems::HandleInputBackend::typeID;
	ecs::systems::HandleInputBackend* psys = rECS.createSystem<ecs::systems::HandleInputBackend>(0);// parameter �r layer

	rECS.createSystem<ecs::systems::HandleKeyboardSystem>(1);
	rECS.createSystem<ecs::systems::HandleMouseSystem>(1);
	rECS.createSystem<ecs::systems::HandleWebSystem>(1);

	//// COMPONENTS

	// Backend Components
	ecs::components::InputBackendComp backend = ecs::components::InputBackendComp();
	backend.backend = inp;
	//backend.backend = new InputBackend();

	// Keyboard Components
	ecs::components::KeyboardComponent kb_comp = ecs::components::KeyboardComponent();

	// Mouse Components
	ecs::components::MouseComponent mouse_comp = ecs::components::MouseComponent();

	// Web Components
	ecs::components::UserButtonComponent user_buttonComp = ecs::components::UserButtonComponent();
	ecs::components::UserTileComponent user_tileComp = ecs::components::UserTileComponent();
	ecs::components::UserCommandComponent user_commandComp = ecs::components::UserCommandComponent();
	
	ecs::BaseComponent* components[] =
	{
		&user_buttonComp,
		& user_tileComp,
		& user_commandComp,
		&mouse_comp,
		&kb_comp,
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