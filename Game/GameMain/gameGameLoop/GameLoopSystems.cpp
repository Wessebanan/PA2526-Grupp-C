#include "GameLoopSystems.h"
#include "GameLoopComponents.h"
#include "GameLoopEvents.h"
#include "../gameAI/AIComponents.h"
#include "../../AI/includes/GridFunctions.h"
#include "GridProp.h"
#include "../Physics/PhysicsComponents.h"
#include "../gameUtility/UtilityComponents.h"

#include "..//gameUtility/Timer.h"

using namespace ecs;
using namespace ecs::components;


/*
====================================================
====================================================
============== UPDATE SYSTEMS ======================
====================================================
====================================================
====================================================
*/

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

	// To be sent to the UI
	p_gl->mRoundTime.GetRoundTime();
	p_gl->mRoundTime.GetFrameTime();
	p_gl->mRoundTime.GetGameTime();

}

///////////////////

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

	int check_any_live = 0;
	PLAYER alive_player;

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
				//break;
			}
		}

		if (check_alive)
		{
			// The armt has a alive unit
			check_any_live++;
			UnitComponent* p_unit = (UnitComponent*)getComponentFromKnownEntity< UnitComponent>(p_army_comp->unitIDs[0]);
			alive_player = p_unit->playerID;
		}
	}

	// There is one winner
	if (check_any_live == 1)
	{
		events::RoundEndEvent eve;
		eve.winner = alive_player;
		createEvent(eve);
		
	}
}

/*
====================================================
====================================================
============== EVENTREADERS ========================
====================================================
====================================================
====================================================
*/


ecs::systems::GameStartSystem::GameStartSystem()
{
	updateType = EventReader;
	typeFilter.addRequirement(ecs::events::GameStartEvent::typeID);
}

ecs::systems::GameStartSystem::~GameStartSystem()
{
}

void ecs::systems::GameStartSystem::readEvent(BaseEvent& event, float delta)
{
	if (event.getTypeID() == ecs::events::GameStartEvent::typeID)
	{

		ComponentIterator itt = getComponentsOfType<GameLoopComponent>();
		GameLoopComponent* p_gl;
		while (p_gl = (GameLoopComponent*)itt.next())
		{
			p_gl->mRoundTime.StartGame();
		}
	}
}

///////////////////


ecs::systems::RoundStartSystem::RoundStartSystem()
{
	updateType = EventReader;
	typeFilter.addRequirement(ecs::events::RoundStartEvent::typeID);
}

ecs::systems::RoundStartSystem::~RoundStartSystem()
{

}

void ecs::systems::RoundStartSystem::readEvent(BaseEvent& event, float delta)
{
	if (event.getTypeID() == ecs::events::RoundStartEvent::typeID)
	{
		ComponentIterator itt = getComponentsOfType<ArmyComponent>();

		ArmyComponent* p_army_comp;
		int2 starting_tile_index;
		ID temp_id;
		ecs::components::TransformComponent* unit_transform;
		ecs::components::TransformComponent* tile_transform;
		GridProp* p_gp = GridProp::GetInstance();
		int i = 0;

		int2 size = p_gp->GetSize();
		int divider = 3;
		// Loop and set all health to full
		while (p_army_comp = (ArmyComponent*)itt.next())
		{
			int u = 0;

			// Get each tile that the players will start on
			starting_tile_index = GridFunctions::FindStartingTile((PLAYER)i, size.x, size.y);
			temp_id = p_gp->mGrid[starting_tile_index.y][starting_tile_index.x].Id;
			tile_transform = getComponentFromKnownEntity<ecs::components::TransformComponent>(temp_id);


			for (size_t i = 0; i < p_army_comp->unitIDs.size(); i++)
			{
				int entID = p_army_comp->unitIDs[i];
				HealthComponent* p_hp = (HealthComponent*)getComponentFromKnownEntity< HealthComponent>(entID);
				unit_transform = (TransformComponent*)getComponentFromKnownEntity< TransformComponent>(entID);

				// Set the helth to the original
				p_hp->mHealth = p_hp->mBaseHealth;

				// Place the units on the tile
				// ----------------------- COPIED FROM THE CREATE ARMIES FUNCTION
				//Set the starting position of the unit depending on the center position of the units starting tile. Needs to
				//be updated if the number of units is increased beyond 3.
				if (u == 0)
				{
					unit_transform->position.x = tile_transform->position.x + (float(TILE_RADIUS) / divider);
					unit_transform->position.y = tile_transform->position.y + 1.1f;
					unit_transform->position.z = tile_transform->position.z + (float(TILE_RADIUS) / divider);
				}
				else if (u == 1)
				{
					unit_transform->position.x = tile_transform->position.x - (float(TILE_RADIUS) / divider);
					unit_transform->position.y = tile_transform->position.y + 1.1f;
					unit_transform->position.z = tile_transform->position.z + (float(TILE_RADIUS) / divider);
				}
				else
				{
					unit_transform->position.x = tile_transform->position.x;
					unit_transform->position.y = tile_transform->position.y + 1.1f;
					unit_transform->position.z = tile_transform->position.z - (float(TILE_RADIUS) / divider);
				}

				u++;
			}

			i++;
		}

		// Start the timer after eveything has been loaded
		itt = getComponentsOfType<GameLoopComponent>();
		GameLoopComponent* p_gl;
		while (p_gl = (GameLoopComponent*)itt.next())
		{
			p_gl->mRoundTime.StartRound();
		}

	}


}

///////////////////

ecs::systems::RoundOverSystem::RoundOverSystem()
{
	updateType = EventReader;
	typeFilter.addRequirement(ecs::events::RoundEndEvent::typeID);
}

ecs::systems::RoundOverSystem::~RoundOverSystem()
{
}

void ecs::systems::RoundOverSystem::readEvent(BaseEvent& event, float delta)
{
	if (event.getTypeID() == ecs::events::RoundEndEvent::typeID)
	{
		int winner = static_cast<ecs::events::RoundEndEvent*>(&event)->winner;

		ComponentIterator itt = getComponentsOfType<GameLoopComponent>();
		GameLoopComponent* p_gl;
		while (p_gl = (GameLoopComponent*)itt.next())
		{
			p_gl->mPlayerPoints[winner]++;
		}

		if (p_gl->mPlayerPoints[winner] < 3)
		{
			// Can be reworked to start prep phase
			events::RoundStartEvent eve;
			createEvent(eve);
		}
	}

}
