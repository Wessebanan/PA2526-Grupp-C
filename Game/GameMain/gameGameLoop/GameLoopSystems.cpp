#include "GameLoopSystems.h"
#include "GameLoopComponents.h"
#include "../gameAI/AIComponents.h"
#include "../Physics/PhysicsComponents.h"

using namespace ecs;
using namespace ecs::components;

ecs::systems::GameLoopSystem::GameLoopSystem()
{
	updateType = ecs::EntityUpdate;
	typeFilter.addRequirement(ecs::components::GameLoopComponent::typeID);
}

ecs::systems::GameLoopSystem::~GameLoopSystem()
{
}

void ecs::systems::GameLoopSystem::updateEntity(FilteredEntity& _entityInfo, float _delta)
{
	GameLoopComponent* p_gl = _entityInfo.getComponent<components::GameLoopComponent>();
	
	ComponentIterator itt = getComponentsOfType<ArmyComponent>();

	ArmyComponent* p_army_comp;

	while (p_army_comp = (ArmyComponent*)itt.next())
	{
		bool check_alive = false;

		for (size_t i = 0; i < p_army_comp->unitIDs.size(); i++)
		{
			int entID = p_army_comp->unitIDs[i];
			HealthComponent* p_hp  = (HealthComponent*)getComponentFromKnownEntity< HealthComponent>(entID);

			if (p_hp > 0)
			{
				check_alive = true;
				break;
			}
		}

		if (check_alive)
		{

		}
	}

}

ecs::systems::GameLoopAliveSystem::GameLoopAliveSystem()
{
	updateType = ecs::EntityUpdate;
	typeFilter.addRequirement(ecs::components::GameLoopComponent::typeID);
}

ecs::systems::GameLoopAliveSystem::~GameLoopAliveSystem()
{
}

void ecs::systems::GameLoopAliveSystem::updateEntity(FilteredEntity& _entityInfo, float _delta)
{
	GameLoopComponent* p_gl = _entityInfo.getComponent<components::GameLoopComponent>();

	ComponentIterator itt = getComponentsOfType<ArmyComponent>();

	ArmyComponent* p_army_comp;

	bool check_any_live = false;

	while (p_army_comp = (ArmyComponent*)itt.next())
	{
		bool check_alive = false;

		for (size_t i = 0; i < p_army_comp->unitIDs.size(); i++)
		{
			int entID = p_army_comp->unitIDs[i];
			HealthComponent* p_hp = (HealthComponent*)getComponentFromKnownEntity< HealthComponent>(entID);

			if (p_hp > 0)
			{
				check_alive = true;
				break;
			}
		}

		if (check_alive)
		{
			// The armt has a alive unit
			check_any_live = true;
			break;
		}
	}

	if (!check_any_live)
	{
		p_gl->mRoundOver = true;
	}
}
