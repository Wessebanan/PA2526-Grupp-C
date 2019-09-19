#include "HandleInputBackend.h"

using namespace ecs;

/// KEYBOARD

ecs::systems::HandleKeyboardSystem::HandleKeyboardSystem()
{
	updateType = ecs::EntityUpdate;
	typeFilter.addRequirement(ecs::components::KeyboardComponent::typeID);
	typeFilter.addRequirement(ecs::components::InputBackendComp::typeID);
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
	updateType = ecs::EntityUpdate;
	typeFilter.addRequirement(ecs::components::MouseComponent::typeID);
	typeFilter.addRequirement(ecs::components::InputBackendComp::typeID);
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

	//if (mouse->diffLength == backendComp->backend->mouse->diffLength && mouse->diffFloat2.x == backendComp->backend->mouse->diffFloat2)
	
	mouse->diffFloat2 = backendComp->backend->mouse->diffFloat2;
	mouse->diffLength = backendComp->backend->mouse->diffLength;

}

/// WEB

ecs::systems::HandleWebSystem::HandleWebSystem()
{
	updateType = ecs::EntityUpdate;
	typeFilter.addRequirement(ecs::components::InputBackendComp::typeID);
	typeFilter.addRequirement(ecs::components::UserButtonComponent::typeID);
	typeFilter.addRequirement(ecs::components::UserTileComponent::typeID);
}

ecs::systems::HandleWebSystem::~HandleWebSystem()
{
}

void ecs::systems::HandleWebSystem::updateEntity(FilteredEntity& _entityInfo, float _delta)
{
	InputBackendComp* backendComp = _entityInfo.getComponent<components::InputBackendComp>();
	UserButtonComponent* button = _entityInfo.getComponent<components::UserButtonComponent>();
	UserTileComponent* tile = _entityInfo.getComponent<components::UserTileComponent>();

	for (int i = 0; i < 4; i++)
	{
		button->buttons[i][0] = backendComp->backend->playerControll[i]->keyU.pressed;
		button->buttons[i][1] = backendComp->backend->playerControll[i]->keyD.pressed;
		button->buttons[i][2] = backendComp->backend->playerControll[i]->keyL.pressed;
		button->buttons[i][3] = backendComp->backend->playerControll[i]->keyR.pressed;

		tile->tile[i][0] = backendComp->backend->players[i]->currButton0;
		tile->tile[i][1] = backendComp->backend->players[i]->currButton1;
	}
}

/// HEADER INPUT 

systems::HandleInputBackend::HandleInputBackend()
{
	updateType = ecs::EntityUpdate;
	typeFilter.addRequirement(ecs::components::InputBackendComp::typeID);


}

systems::HandleInputBackend::~HandleInputBackend()
{
}

void ecs::systems::HandleInputBackend::updateEntity(FilteredEntity& _entityInfo, float _delta)
{
	InputBackendComp* backendComp = _entityInfo.getComponent<components::InputBackendComp>();

	backendComp->backend->updateKeyboard();
	backendComp->backend->updateMouse();
	backendComp->backend->updateWeb();

}