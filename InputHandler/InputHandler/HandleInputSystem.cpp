#include "HandleInputSystem.h"

using namespace ecs;

/// KEYBOARD

ecs::systems::HandleKeyboardSystem::HandleKeyboardSystem()
{
	updateType = ecs::EntityUpdate;
	componentFilter.addRequirement(ecs::components::KeyboardComponent::typeID);
	componentFilter.addRequirement(ecs::components::InputBackendComp::typeID);
}

ecs::systems::HandleKeyboardSystem::~HandleKeyboardSystem()
{
}

void ecs::systems::HandleKeyboardSystem::updateEntity(FilteredEntity& _entityInfo, float _delta)
{
	KeyboardComponent* kb = _entityInfo.getComponent<components::KeyboardComponent>();
	InputBackendComp* backendComp = _entityInfo.getComponent<components::InputBackendComp>();

	kb->W = backendComp->backend->wsad->keyU.pressed;
	kb->S = backendComp->backend->wsad->keyD.pressed;
	kb->A = backendComp->backend->wsad->keyL.pressed;
	kb->D = backendComp->backend->wsad->keyR.pressed;

	kb->R = backendComp->backend->ressetKey->key.pressed;
	kb->ECS = backendComp->backend->exitKey->key.pressed;

	kb->Q = false;
	kb->E = false;

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
	MouseComponent* mouse = _entityInfo.getComponent<components::MouseComponent>();
	InputBackendComp* backendComp = _entityInfo.getComponent<components::InputBackendComp>();

	mouse->LMB = backendComp->backend->mouseLKey->key.pressed;
	mouse->RMB = backendComp->backend->mouseRKey->key.pressed;

	mouse->pos = backendComp->backend->mouse->newPos;
	mouse->diffFloat2 = backendComp->backend->mouse->diffFloat2;
	mouse->diffLength = backendComp->backend->mouse->diffLength;
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
	backendComp->backend->updateMouse();
	//backendComp->backend->updateWeb();

}