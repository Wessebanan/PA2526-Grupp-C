#include "includes/InterpretWebSystem.h"

using namespace ecs;
using namespace ecs::components;

ecs::systems::ChangeFSMSystem::ChangeFSMSystem()
{
	updateType = ecs::EntityUpdate;
	typeFilter.addRequirement(ecs::components::UserCommandComponent::typeID);
}

ecs::systems::ChangeFSMSystem::~ChangeFSMSystem()
{
}

void ecs::systems::ChangeFSMSystem::updateEntity(FilteredEntity& _entityInfo, float _delta)
{
	UserCommandComponent* ucComp = _entityInfo.getComponent<UserCommandComponent>();

	// state component 

	if (ucComp)
	{
		for (size_t i = 0; i < 4; i++)
		{
			if (ucComp->userCommands[i].mCommand == "move")
			{
				// change state component
			}
			else if (ucComp->userCommands[i].mCommand == "idel")
			{
				// change state component
			}
		}
		
		
	}

}
