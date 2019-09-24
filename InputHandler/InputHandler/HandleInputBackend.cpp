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

	kb->W = backendComp->backend->mpWsad->keyU.pressed;
	kb->S = backendComp->backend->mpWsad->keyD.pressed;
	kb->A = backendComp->backend->mpWsad->keyL.pressed;
	kb->D = backendComp->backend->mpWsad->keyR.pressed;

	kb->R = backendComp->backend->mpRessetKey->key.pressed;
	kb->ECS = backendComp->backend->mpExitKey->key.pressed;

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

	

	mouse->LMB = backendComp->backend->mpMouseLKey->key.pressed;
	mouse->RMB = backendComp->backend->mpMouseRKey->key.pressed;

	mouse->pos = backendComp->backend->mpMouse->mNewPos;

	//if (mouse->diffLength == backendComp->backend->mouse->diffLength && mouse->diffFloat2.x == backendComp->backend->mouse->diffFloat2)
	
	mouse->diffFloat2 = backendComp->backend->mpMouse->mDiffFloat2;
	mouse->diffLength = backendComp->backend->mpMouse->mDiffLength;

}

/// WEB

ecs::systems::HandleWebSystem::HandleWebSystem()
{
	updateType = ecs::EntityUpdate;
	typeFilter.addRequirement(ecs::components::InputBackendComp::typeID);
	typeFilter.addRequirement(ecs::components::UserButtonComponent::typeID);
	typeFilter.addRequirement(ecs::components::UserTileComponent::typeID);
	typeFilter.addRequirement(ecs::components::UserCommandComponent::typeID);
}

ecs::systems::HandleWebSystem::~HandleWebSystem()
{
}

void ecs::systems::HandleWebSystem::updateEntity(FilteredEntity& _entityInfo, float _delta)
{
	InputBackendComp* backendComp = _entityInfo.getComponent<components::InputBackendComp>();
	UserButtonComponent* buttonComp = _entityInfo.getComponent<components::UserButtonComponent>();
	UserTileComponent* tileComp = _entityInfo.getComponent<components::UserTileComponent>();
	UserCommandComponent* commandComp = _entityInfo.getComponent<components::UserCommandComponent>();

	for (int i = 0; i < 4; i++)
	{
		buttonComp->userButtons[i].mButton = backendComp->backend->mpUserButton[i]->mButton;

		tileComp->userTiles[i].mCordX = backendComp->backend->mpUserTile[i]->mCordX;
		tileComp->userTiles[i].mCordY = backendComp->backend->mpUserTile[i]->mCordY;

		commandComp->userCommands[i].mCommand = backendComp->backend->mpUserCommand[i]->mCommand;
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