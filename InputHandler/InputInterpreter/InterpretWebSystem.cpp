#include "includes/InterpretWebSystem.h"

using namespace ecs;
using namespace ecs::components;

ecs::systems::ChangeFSMSystem::ChangeFSMSystem()
{
	updateType = ecs::EntityUpdate;
	typeFilter.addRequirement(ecs::components::UserCommandComponent::typeID);

	for (size_t i = 0; i < 4; i++)
	{
		mCurrStates[i] = FSMStates::ERR;
	}
}

ecs::systems::ChangeFSMSystem::~ChangeFSMSystem()
{
}

void ecs::systems::ChangeFSMSystem::updateEntity(FilteredEntity& _entityInfo, float _delta)
{
	UserCommandComponent* ucComp = _entityInfo.getComponent<UserCommandComponent>();


	if (ucComp)
	{
		for (size_t i = 0; i < 4; i++)
		{
			if (ucComp->userCommands[i].mCommand == "move" && mCurrStates[i] != FSMStates::MOVE)
			{
				// change state component
				events::ChangeUserStateEvent tempEvent;
				tempEvent.playerId = i;
				tempEvent.newState = FSMStates::MOVE;

				mCurrStates[i] = FSMStates::MOVE;

				createEvent(tempEvent);
			}
			else if (ucComp->userCommands[i].mCommand == "idle" && mCurrStates[i] != FSMStates::IDLE)
			{
				// change state component
				events::ChangeUserStateEvent tempEvent;
				tempEvent.playerId = i;
				tempEvent.newState = FSMStates::IDLE;

				mCurrStates[i] = FSMStates::IDLE;

				createEvent(tempEvent);
			}
		}
		
		
	}

}
