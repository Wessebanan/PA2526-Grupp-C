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

#include "AIGlobals.h"
#include "..//gameAnimation/AnimationComponents.h"

#include "..//MeshContainer/MeshContainer.h"	

using namespace ecs;
using namespace ecs::components;


// Creates a weapon out of a mesh and weapon type. (weapon, transform and mesh components)
ecs::Entity* CreateWeaponEntity(ModelLoader::Mesh* pMesh, WEAPON_TYPE weaponType, ID ownerEntity = 0);

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
		if (p_army_comp->unitIDs.size() > 0)
		{
			check_any_live++;
			alive_player = p_army_comp->playerID;
		}


		//bool check_alive = false;

		//for (size_t i = 0; i < p_army_comp->unitIDs.size(); i++)
		//{
		//	int entID = p_army_comp->unitIDs[i];
		//	HealthComponent* p_hp = (HealthComponent*)getComponentFromKnownEntity< HealthComponent>(entID);

		//	if (p_hp > 0)
		//	{
		//		check_alive = true;
		//		//break;
		//	}
		//}

		//if (check_alive)
		//{
		//	// The armt has a alive unit
		//	check_any_live++;
		//	UnitComponent* p_unit = (UnitComponent*)getComponentFromKnownEntity< UnitComponent>(p_army_comp->unitIDs[0]);
		//	alive_player = p_unit->playerID;
		//}
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
		this->CreateUnitPhysics();

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
		for (size_t kk = 0; kk < p_army->unitIDs.size(); kk++)
		{

			ecs::components::EquipmentComponent* p_eq = getComponentFromKnownEntity<ecs::components::EquipmentComponent>(p_army->unitIDs[kk]);

			removeEntity(p_eq->mEquippedWeapon);
			removeEntity(p_army->unitIDs[kk]);
		}
		p_army->unitIDs.clear();


		////Fetch the index of the starting tile for this player.
		starting_tile_index = GridFunctions::FindStartingTile(p_army->playerID, size.x, size.y, MAPINITSETTING::HOLMES);
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

			// Create and init skeleton comp

			ecs::components::SkeletonComponent skele_comp;

			//ModelLoader::UniqueSkeletonData* skeletonData = &s.getComponent<ecs::components::SkeletonComponent>()->skeletonData;
			//skeletonData->Init(MeshContainer::GetMeshCPU(MESH_TYPE::MESH_TYPE_UNIT)->GetSkeleton());
			//skeletonData->StartAnimation(ModelLoader::ANIMATION_TYPE::IDLE);

			temp_entity = createEntity(transform, unit, idle_state, color_comp, skele_comp); //
			p_army->unitIDs.push_back(temp_entity->getID());
		}
		i++;


	}

	// INIT ANIMATIONS

	ecs::TypeFilter skeleton_filter;
	skeleton_filter.addRequirement(ecs::components::SkeletonComponent::typeID);
	ecs::EntityIterator skeletons = getEntitiesByFilter(skeleton_filter);

	// Initalize the skeleton data structs and start the Idle animation as default
	for (ecs::FilteredEntity s : skeletons.entities)
	{
		ModelLoader::UniqueSkeletonData* skeletonData = &s.getComponent<ecs::components::SkeletonComponent>()->skeletonData;
		skeletonData->Init(MeshContainer::GetMeshCPU(MESH_TYPE::MESH_TYPE_UNIT)->GetSkeleton());
		skeletonData->StartAnimation(ModelLoader::ANIMATION_TYPE::IDLE);
	}


}

void ecs::systems::RoundStartSystem::CreateUnitPhysics()
{
	ecs::TypeFilter filter;
	filter.addRequirement(UnitComponent::typeID);
	ecs::EntityIterator it = getEntitiesByFilter(filter);
	
	ModelLoader::Mesh* pMesh = MeshContainer::GetMeshCPU(MESH_TYPE_UNIT);

	MeshComponent mesh_component;
	mesh_component.mMesh = pMesh;
	ObjectCollisionComponent object_collision;
	GroundCollisionComponent ground_collision;
	DynamicMovementComponent movement_component;
	HealthComponent health_component;
	EquipmentComponent equipment_component;

	for (int i = 0; i < it.entities.size(); i++)
	{
		ecs::Entity* current = it.entities.at(i).entity;
		if (!current->hasComponentOfType<MeshComponent>())
		{
			createComponent<MeshComponent>(current->getID(), mesh_component);
		}

		if (!current->hasComponentOfType<ObjectCollisionComponent>())
		{
			createComponent<ObjectCollisionComponent>(current->getID(), object_collision);
		}

		if (!current->hasComponentOfType<GroundCollisionComponent>())
		{
			createComponent<GroundCollisionComponent>(current->getID(), ground_collision);
		}

		if (!current->hasComponentOfType<DynamicMovementComponent>())
		{
			createComponent<DynamicMovementComponent>(current->getID(), movement_component);
		}

		if (!current->hasComponentOfType<HealthComponent>())
		{
			createComponent<HealthComponent>(current->getID(), health_component);
		}

		// Initializing with fist weapons.
		if (!current->hasComponentOfType<EquipmentComponent>())
		{
			// Setting melee range here (arm length) hoping that any unit mesh is either facing x or z on load.
			ObjectCollisionComponent* p_object_collision = dynamic_cast<ObjectCollisionComponent*>(getComponent(ObjectCollisionComponent::typeID, current->getComponentID(ObjectCollisionComponent::typeID)));
			XMFLOAT3 extents = p_object_collision->mAABB.Extents;

			// TEMP multiplying extents by inverse of scale given in init system for object
			// collision component for a more snug hitbox.
			extents.x *= 1.0f / 0.3f;
			extents.z *= 1.0f / 0.9f;
			equipment_component.mMeleeRange = extents.x > extents.z ? extents.x : extents.z;
			TransformComponent* p_transform = dynamic_cast<TransformComponent*>(getComponent(TransformComponent::typeID, current->getComponentID(TransformComponent::typeID)));

			// Assume uniform scale (pain otherwise).
			equipment_component.mMeleeRange *= p_transform->scale.x;

			// Set attack range to melee range since fist adds no range.
			equipment_component.mAttackRange = equipment_component.mMeleeRange;


			WeaponComponent		weapon_component;
			TransformComponent	weapon_transform_component;
			MeshComponent		weapon_mesh_component;

			WEAPON_TYPE weaponType = WEAPON_TYPE::FIST;

			weapon_transform_component.scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
			weapon_component.mType = FIST;
			weapon_component.mOwnerEntity = current->getID();

			switch (weaponType)
			{
			case SWORD:
			{
				ModelLoader::Mesh sword("Physics/TestModel/sword.fbx");
				weapon_mesh_component.mMesh = pMesh;
				break;
			}
			case FIST:
				weapon_mesh_component.mMesh = pMesh;
				break;
			case PROJECTILE:
				MessageBoxA(NULL, "Projectile weapon not yet implemented.", NULL, MB_YESNO);
				break;
			}

			ecs::Entity* weapon_entity = createEntity(weapon_mesh_component, weapon_transform_component, weapon_component);

			equipment_component.mEquippedWeapon = weapon_entity->getID();
			createComponent<EquipmentComponent>(current->getID(), equipment_component);
		}
	}
}

inline ecs::Entity* CreateWeaponEntity(ModelLoader::Mesh* pMesh, WEAPON_TYPE weaponType, ID ownerEntity)
{
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
				cout << "The round winner is Player " << winner << endl;
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
