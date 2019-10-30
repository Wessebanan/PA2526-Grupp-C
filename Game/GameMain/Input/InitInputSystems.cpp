#include "InitInputSystems.h"
#include "..//gameAnimation/AnimationEvents.h"

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
	KeyboardComponent* p_kb = _entityInfo.getComponent<components::KeyboardComponent>();
	InputBackendComp* p_backend_comp = _entityInfo.getComponent<components::InputBackendComp>();

	// saves all keyboard keys, the mouse keys are done in mouse system
	p_kb->W = p_backend_comp->backend->mpWsad->keyU.pressed;
	p_kb->S = p_backend_comp->backend->mpWsad->keyD.pressed;
	p_kb->A = p_backend_comp->backend->mpWsad->keyL.pressed;
	p_kb->D = p_backend_comp->backend->mpWsad->keyR.pressed;

	p_kb->R = p_backend_comp->backend->mpRessetKey->key.pressed;
	p_kb->ECS = p_backend_comp->backend->mpExitKey->key.pressed;

	p_kb->Q = false;
	p_kb->E = false;

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
	MouseComponent* p_mouse = _entityInfo.getComponent<components::MouseComponent>();
	InputBackendComp* p_backend_comp = _entityInfo.getComponent<components::InputBackendComp>();


	// the mouse buttons
	p_mouse->RMB = p_backend_comp->backend->mpMouseRKey->key.pressed;
	p_mouse->LMB = p_backend_comp->backend->mpMouseLKey->key.pressed;
	
	// the current position
	p_mouse->pos = p_backend_comp->backend->mpMouse->mNewPos;
	
	// the delta values
	p_mouse->diffFloat2 = p_backend_comp->backend->mpMouse->mDiffFloat2;
	p_mouse->diffLength = p_backend_comp->backend->mpMouse->mDiffLength;

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
	InputBackendComp* p_backend_comp = _entityInfo.getComponent<components::InputBackendComp>();
	UserButtonComponent* p_button_comp = _entityInfo.getComponent<components::UserButtonComponent>();
	UserTileComponent* p_tile_comp = _entityInfo.getComponent<components::UserTileComponent>();
	UserCommandComponent* p_command_comp = _entityInfo.getComponent<components::UserCommandComponent>();

	for (int i = 0; i < 4; i++)
	{
		p_button_comp->userButtons[i].mButton = p_backend_comp->backend->mpUserButton[i]->mButton;

		p_tile_comp->userTiles[i].mCordX = p_backend_comp->backend->mpUserTile[i]->mCordX;
		p_tile_comp->userTiles[i].mCordY = p_backend_comp->backend->mpUserTile[i]->mCordY;

		p_command_comp->userCommands[i].mCommand = p_backend_comp->backend->mpUserCommand[i]->mCommand;

		if (*p_backend_comp->backend->mpUserPing[i] == true)
		{
			ecs::events::PingEvent eve;
			eve.playerId = (PLAYER)i;
			createEvent(eve);
		}
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

	InputBackendComp* p_backend_comp = _entityInfo.getComponent<components::InputBackendComp>();

	// keyboard input 
	p_backend_comp->backend->updateKeyboard();
	// mouse input
	p_backend_comp->backend->updateMouse();
	// web input from users
	p_backend_comp->backend->updateWeb();

}