#include "../Input/InterpretWebSystems.h"

using namespace ecs;
using namespace ecs::components;

ecs::systems::ChangeFSMSystem::ChangeFSMSystem()
{
	updateType = ecs::EntityUpdate;
	typeFilter.addRequirement(ecs::components::UserCommandComponent::typeID);

	for (size_t i = 0; i < 4; i++)
	{
		mCurrStates[i] = STATE::IDLE;
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
			if (ucComp->userCommands[i].mCommand == "move" && mCurrStates[i] != STATE::MOVE)
			{
				// change state component
				events::ChangeUserStateEvent cus_event;
				cus_event.newState = STATE::MOVE;
				cus_event.playerId = (PLAYER)i;

				mCurrStates[i] = STATE::MOVE;

				createEvent(cus_event);
			}
			else if (ucComp->userCommands[i].mCommand == "idle" && mCurrStates[i] != STATE::IDLE)
			{
				// change state component
				events::ChangeUserStateEvent cus_event;
				cus_event.newState = STATE::IDLE;
				cus_event.playerId = (PLAYER)i;

				mCurrStates[i] = STATE::IDLE;

				createEvent(cus_event);
			}
			else if (ucComp->userCommands[i].mCommand == "attack" && mCurrStates[i] != STATE::ATTACK)
			{
				// change state component
				events::ChangeUserStateEvent cus_event;
				cus_event.newState = STATE::ATTACK;
				cus_event.playerId = (PLAYER)i;

				mCurrStates[i] = STATE::ATTACK;

				createEvent(cus_event);
			}
			else if (ucComp->userCommands[i].mCommand == "loot" && mCurrStates[i] != STATE::LOOT)
			{
				// change state component
				events::ChangeUserStateEvent cus_event;
				cus_event.newState = STATE::LOOT;
				cus_event.playerId = (PLAYER)i;

				mCurrStates[i] = STATE::LOOT;

				createEvent(cus_event);
			}
		}
	}
}
