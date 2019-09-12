#include "HandleInputSystem.h"

using namespace ecs;

/// KEYBOARD

ecs::systems::HandleKeyboardSystem::HandleKeyboardSystem()
{
}

ecs::systems::HandleKeyboardSystem::~HandleKeyboardSystem()
{
}

void ecs::systems::HandleKeyboardSystem::updateEntity(FilteredEntity& _entityInfo, float _delta)
{
}

/// MOUSE

ecs::systems::HandleMouseSystem::HandleMouseSystem()
{
}

ecs::systems::HandleMouseSystem::~HandleMouseSystem()
{
}

void ecs::systems::HandleMouseSystem::updateEntity(FilteredEntity& _entityInfo, float _delta)
{
}

/// WEB

ecs::systems::HandleWebSystem::HandleWebSystem()
{
}

ecs::systems::HandleWebSystem::~HandleWebSystem()
{
}

void ecs::systems::HandleWebSystem::updateEntity(FilteredEntity& _entityInfo, float _delta)
{
}

/// HEADER INPUT 

systems::HandleInputSystem::HandleInputSystem()
{
	updateType = ecs::EntityUpdate;
	componentFilter.addRequirement(ecs::components::InputBackendComp::typeID);


}

systems::HandleInputSystem::~HandleInputSystem()
{
}

void ecs::systems::HandleInputSystem::updateEntity(FilteredEntity& _entityInfo, float _delta)
{
	InputBackendComp* backendComp = _entityInfo.getComponent<components::InputBackendComp>();

	backendComp->backend->updateKeyboard();
	//backendComp->backend->updateMouse();
	//backendComp->backend->updateWeb();

	std::cout << backendComp->backend->wsad->keyU.pressed;
}