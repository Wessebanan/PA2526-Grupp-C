#include "GameLoopSystems.h"
#include "GameLoopComponents.h"
#include "GameLoopEvents.h"
#include "../gameAI/AIComponents.h"
#include "../../AI/includes/GridFunctions.h"
#include "GridProp.h"
#include "../Physics/PhysicsComponents.h"
#include "../gameUtility/UtilityComponents.h"

#include "..//gameUtility/Timer.h"
#include "..//Input/InterpretWebEvents.h"

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
		//events::RoundEndEvent eve;
		//eve.winner = alive_player;
		//createEvent(eve);
		
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

		ecs::events::RoundStartEvent eve;
		createEvent(eve);
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
		this->CreateUnits();

		// Start the timer after eveything has been loaded
		ComponentIterator itt = getComponentsOfType<GameLoopComponent>();
		GameLoopComponent* p_gl;
		while (p_gl = (GameLoopComponent*)itt.next())
		{
			p_gl->mRoundTime.StartRound();
		}


		ChangeUserStateEvent e;
		e.newState = ATTACK;
		e.playerId = PLAYER1;
		createEvent(e);
		e.playerId = PLAYER2;
		createEvent(e);
		e.playerId = PLAYER3;
		createEvent(e);
		e.playerId = PLAYER4;
		createEvent(e);


	}


}

void ecs::systems::RoundStartSystem::CreateUnits()
{
	/* TEAM COLORS */
	struct uint3
	{
		UINT r, g, b;
	};

	uint3 army_colors[4];

	army_colors[0] = { 200,   0,   0 };	// Red		Army 1
	army_colors[1] = { 20,  20,  20 };	// Gray		Army 2
	army_colors[2] = { 0, 100, 100 };	// Cyan		Army 3
	army_colors[3] = { 100,   0, 100 };	// Purple	Army 4

	/* END	*/


	//Create Components for a "Unit" entity.
	ecs::components::TransformComponent transform;
	ecs::components::UnitComponent unit;
	ecs::components::IdleStateComponent idle_state;
	ecs::components::ColorComponent color_comp;
	//Temporary entity pointer so that we can fetch the units IDs so that we can store
	//them in the army component.
	ecs::Entity* temp_entity;
	int2 starting_tile_index;
	ID temp_id;
	ecs::components::TransformComponent* p_transform;
	GridProp* p_gp = GridProp::GetInstance();
	int2 size = p_gp->GetSize();
	//Loop for every player.
	ecs::ComponentIterator itt = getComponentsOfType(ecs::components::ArmyComponent::typeID);
	ecs::components::ArmyComponent* p_army;
	int i = 0;
	while (p_army = (ecs::components::ArmyComponent*)itt.next())
	{
		// Clear it out if there was an
		p_army->unitIDs.clear();

		////Fetch the index of the starting tile for this player.
		starting_tile_index = GridFunctions::FindStartingTile(p_army->playerID, size.x, size.y);
		temp_id = p_gp->mGrid[starting_tile_index.y][starting_tile_index.x].Id;
		p_transform = getComponentFromKnownEntity<ecs::components::TransformComponent>(temp_id);
		//Set current players enum ID for this armies units.
		unit.playerID = p_army->playerID;
		int divider = 3;
		for (int u = 0; u < PlayerProperties::numberOfUnits; u++)
		{
			//Set the starting position of the unit depending on the center position of the units starting tile. Needs to
			//be updated if the number of units is increased beyond 3.
			if (u == 0)
			{
				transform.position.x = p_transform->position.x + (float(TILE_RADIUS) / divider);
				transform.position.y = p_transform->position.y + 1.1f;
				transform.position.z = p_transform->position.z + (float(TILE_RADIUS) / divider);
			}
			else if (u == 1)
			{
				transform.position.x = p_transform->position.x - (float(TILE_RADIUS) / divider);
				transform.position.y = p_transform->position.y + 1.1f;
				transform.position.z = p_transform->position.z + (float(TILE_RADIUS) / divider);
			}
			else
			{
				transform.position.x = p_transform->position.x;
				transform.position.y = p_transform->position.y + 1.1f;
				transform.position.z = p_transform->position.z - (float(TILE_RADIUS) / divider);
			}

			// set scale to fit on tile
			transform.scale.x = 0.1f;
			transform.scale.y = 0.1f;
			transform.scale.z = 0.1f;


			color_comp.red = army_colors[i].r;
			color_comp.green = army_colors[i].g;
			color_comp.blue = army_colors[i].b;

			temp_entity = createEntity(transform, unit, idle_state, color_comp); //
			p_army->unitIDs.push_back(temp_entity->getID());
		}
		i++;
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
		int winner = dynamic_cast<ecs::events::RoundEndEvent*>(&event)->winner;

		if (winner >= 0)
		{
			ComponentIterator itt = ecs::ECSUser::getComponentsOfType(ecs::components::GameLoopComponent::typeID);
			GameLoopComponent* p_gl;
			while (p_gl = static_cast<GameLoopComponent*>(itt.next()))
			{
				p_gl->mPlayerPoints[winner]++;

				// Check if the winner has won enougth to win the game
				if (p_gl->mPlayerPoints[winner] < ROUNDS_TO_WIN)
				{
					// Can be reworked to start prep phase
					events::RoundStartEvent eve;
					createEvent(eve);
				}
			}
		}
	}
}
