#include "InitInputSystems.h"

using namespace ecs;
using namespace ecs::components;

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
	InputBackendComp* backend_comp = _entityInfo.getComponent<components::InputBackendComp>();

	// saves all keyboard keys, the mouse keys are done in mouse system
	kb->W = backend_comp->backend->mpWsad->keyU.pressed;
	kb->S = backend_comp->backend->mpWsad->keyD.pressed;
	kb->A = backend_comp->backend->mpWsad->keyL.pressed;
	kb->D = backend_comp->backend->mpWsad->keyR.pressed;

	kb->R = backend_comp->backend->mpRessetKey->key.pressed;
	kb->ECS = backend_comp->backend->mpExitKey->key.pressed;

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
	InputBackendComp* backend_comp = _entityInfo.getComponent<components::InputBackendComp>();


	//// the mouse buttons
	//mouse->LMB = backend_comp->backend->mpMouseLKey->key.pressed;
	//mouse->RMB = backend_comp->backend->mpMouseRKey->key.pressed;

	//// the current position
	//mouse->pos = backend_comp->backend->mpMouse->mNewPos;


	//// the delta values
	//mouse->diffFloat2 = backend_comp->backend->mpMouse->mDiffFloat2;
	//mouse->diffLength = backend_comp->backend->mpMouse->mDiffLength;

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
	// Each diffrent button
	InputBackendComp* backend_comp = _entityInfo.getComponent<components::InputBackendComp>();
	UserButtonComponent* button_comp = _entityInfo.getComponent<components::UserButtonComponent>();
	UserTileComponent* tile_comp = _entityInfo.getComponent<components::UserTileComponent>();
	UserCommandComponent* command_comp = _entityInfo.getComponent<components::UserCommandComponent>();

	for (int i = 0; i < 4; i++)
	{
		button_comp->userButtons[i].mButton = backend_comp->backend->mpUserButton[i]->mButton;

		tile_comp->userTiles[i].mCordX = backend_comp->backend->mpUserTile[i]->mCordX;
		tile_comp->userTiles[i].mCordY = backend_comp->backend->mpUserTile[i]->mCordY;

		command_comp->userCommands[i].mCommand = backend_comp->backend->mpUserCommand[i]->mCommand;
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
	// updates the components with the data from the backend

	InputBackendComp* backend_comp = _entityInfo.getComponent<components::InputBackendComp>();

	// keyboard input 
	backend_comp->backend->updateKeyboard();
	// mouse input
	backend_comp->backend->updateMouse();
	// web input from users
	backend_comp->backend->updateWeb();

}