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

#include "..//gameAnimation/AnimationEvents.h"
#include "..//UI/UIComponents.h"

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
	typeFilter.addRequirement(ecs::components::UITextComponent::typeID);
}

ecs::systems::GameLoopSystem::~GameLoopSystem()
{
}

// Runs neccesary gameloops, timers etc
void ecs::systems::GameLoopSystem::updateEntity(FilteredEntity& _entityInfo, float _delta)
{
	GameLoopComponent* p_gl = _entityInfo.getComponent<components::GameLoopComponent>();
	UITextComponent* p_text = _entityInfo.getComponent<components::UITextComponent>();

	static float total_time;
	static int total_frames;

	total_time += _delta;
	total_frames++;
	static float framerate_to_print = 0.0f;
	static float frametime_to_print = 0.0f;
	if (total_frames % 100 == 0)
	{
		framerate_to_print = (float)total_frames / total_time;
		frametime_to_print = total_time / (float)total_frames;
		total_frames = 0;
		total_time = 0.0f;
	}

	if (p_text)
	{
		string ss = "";
	
	
		// To be sent to the UI
		//ss.append("ROUNDTIME: ");
		//ss.append(to_string(p_gl->mRoundTime.GetRoundTime()));
		ss.append("\nFRAMERATE: ");
		ss.append(to_string(framerate_to_print));
		ss.append("\nFRAMETIME: ");
		ss.append(to_string(frametime_to_print));
		//ss.append("\nGAMETIME: ");
		//ss.append(to_string(p_gl->mRoundTime.GetGameTime()));
	
		p_text->mStrText = ss;
	}
}

///////////////////



ecs::systems::WaitForStartupSystem::WaitForStartupSystem()
{
	updateType = ecs::EntityUpdate;
	typeFilter.addRequirement(ecs::components::InputBackendComp::typeID);
}

ecs::systems::WaitForStartupSystem::~WaitForStartupSystem()
{
}

void ecs::systems::WaitForStartupSystem::updateEntity(FilteredEntity& _entityInfo, float _delta)
{
	InputBackendComp* p_ib = _entityInfo.getComponent<InputBackendComp>();
	if (p_ib)
	{
		if (p_ib->backend->checkReadyCheck())
		{
			// Starts the first round, should be removed when prepphase is implemented
			ecs::events::RoundStartEvent eve;
			createEvent(eve);

			// Remove itself
			RemoveSystem(WaitForStartupSystem::typeID);
		}
	}
}


///////////////////

ecs::systems::PrepphaseSystem::PrepphaseSystem()
{
	updateType = ecs::EntityUpdate;
	typeFilter.addRequirement(ecs::components::InputBackendComp::typeID);
}

ecs::systems::PrepphaseSystem::~PrepphaseSystem()
{
}

void ecs::systems::PrepphaseSystem::updateEntity(FilteredEntity& _entityInfo, float _delta)
{
	InputBackendComp* p_ib = _entityInfo.getComponent<InputBackendComp>();
	if(p_ib)
	{
		if (p_ib->backend->checkReadyCheck())
		{
			// Starts the first round, should be removed when prepphase is implemented
			ecs::events::RoundStartEvent eve;
			createEvent(eve);

			// Remove itself
			RemoveSystem(PrepphaseSystem::typeID);
		}
	}
}

/////////////////////

ecs::systems::BattlephaseSystem::BattlephaseSystem()
{
	updateType = ecs::MultiEntityUpdate;
	typeFilter.addRequirement(ecs::components::ArmyComponent::typeID);
}

ecs::systems::BattlephaseSystem::~BattlephaseSystem()
{
}

void ecs::systems::BattlephaseSystem::updateMultipleEntities(EntityIterator& _entities, float _delta)
{
	int check_any_live = 0;
	PLAYER alive_player;
	ArmyComponent* p_army_comp;
	//for (size_t i = 0; i < list.size(); i++)
	for (FilteredEntity& army : _entities.entities)
	{
		p_army_comp = army.getComponent<ArmyComponent>();

		if (p_army_comp->unitIDs.size() > 0)
		{
			check_any_live++;
			alive_player = p_army_comp->playerID;
		}
	}

	// There is one winner
	if (check_any_live == 1)
	{
		events::RoundEndEvent eve;
		eve.winner = alive_player;
		createEvent(eve);
	} // Draw
	else if (check_any_live == 0)
	{
		events::RoundEndEvent eve;
		eve.winner = -1;
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
		ComponentIterator itt;
		itt = getComponentsOfType<GameLoopComponent>();
		GameLoopComponent* p_gl;
		while (p_gl = (GameLoopComponent*)itt.next())
		{
			p_gl->mRoundTime.StartGame();

			p_gl->mPlayerPoints[0] = 0;
			p_gl->mPlayerPoints[1] = 0;
			p_gl->mPlayerPoints[2] = 0;
			p_gl->mPlayerPoints[3] = 0;
		}

		// Creating quad tree entity for collision.
		QuadTreeComponent quad_tree;
		int2 grid_size = GridProp::GetInstance()->GetSize();
		createEntity(quad_tree);
		// Puts the players into waiting phase
		itt = getComponentsOfType<InputBackendComp>();
		InputBackendComp* p_ib;
		while (p_ib = (InputBackendComp*)itt.next())
		{
			p_ib->backend->changeGamestate(WEBGAMESTATE::WAITING);
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
		ComponentIterator itt;
		itt = getComponentsOfType<InputBackendComp>();
		InputBackendComp* p_ib;
		while (p_ib = (InputBackendComp*)itt.next())
		{
			p_ib->backend->changeGamestate(WEBGAMESTATE::BATTLEPHASE);
		}


		this->CreateUnits();
		this->CreateUnitPhysics();

		// Start the timer after eveything has been loaded
		itt = getComponentsOfType<GameLoopComponent>();
		GameLoopComponent* p_gl;
		while (p_gl = (GameLoopComponent*)itt.next())
		{
			p_gl->mRoundTime.StartRound();
		}

		ComponentIterator it = ecs::ECSUser::getComponentsOfType(PlayerStateComponent::typeID);
		PlayerStateComponent* p_player_state_comp = static_cast<PlayerStateComponent*>(it.next());
		for (int i = 0; i < 4; i++)
		{
			p_player_state_comp->mCurrentStates[i] = IDLE;
		}

		itt = getComponentsOfType<UITextComponent>();
		UITextComponent* text_comp;
		while (text_comp = (UITextComponent*)itt.next())
		{
			if (text_comp->tag == UITAG::STARTTEXT)
			{
				text_comp->mStrText = "";
			}
		}

		// Create Battlephase system
		CreateSystem<systems::BattlephaseSystem>(1);

		/**************************************/
		/********** USED FOR DEBUG ***********/
		/************************************/
		//ChangeUserStateEvent e;
		//e.newState = ATTACK;
		//e.playerId = PLAYER1;
		//createEvent(e);
		//e.playerId = PLAYER2;
		//createEvent(e);
		//e.playerId = PLAYER3;
		//createEvent(e);
		//e.playerId = PLAYER4;
		//createEvent(e);


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


	// Player 1 - Red
	army_colors[0].r = 117;
	army_colors[0].g = 1;
	army_colors[0].b = 1;

	// Player 2 - Purple
	army_colors[1].r = 74;
	army_colors[1].g = 1;
	army_colors[1].b = 117;

	// Player 3 - Blue
	army_colors[2].r = 47;
	army_colors[2].g = 62;
	army_colors[2].b = 236;

	// Player 4 - Green
	army_colors[3].r = 0;
	army_colors[3].g = 93;
	army_colors[3].b = 5;

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
				transform.position.y = p_transform->position.y + 10.1f;
				transform.position.z = p_transform->position.z + (float(TILE_RADIUS) / divider);
			}
			else if (u == 1)
			{
				transform.position.x = p_transform->position.x - (float(TILE_RADIUS) / divider);
				transform.position.y = p_transform->position.y + 10.1f;
				transform.position.z = p_transform->position.z + (float(TILE_RADIUS) / divider);
			}
			else
			{
				transform.position.x = p_transform->position.x;
				transform.position.y = p_transform->position.y + 10.1f;
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
		skeletonData->Init(MeshContainer::GetMeshCPU(GAME_OBJECT_TYPE_UNIT)->GetSkeleton());
		skeletonData->StartAnimation(ModelLoader::ANIMATION_TYPE::IDLE);
	}
}

void ecs::systems::RoundStartSystem::CreateUnitPhysics()
{
	ecs::TypeFilter filter;
	filter.addRequirement(UnitComponent::typeID);
	ecs::EntityIterator it = getEntitiesByFilter(filter);
	
	ModelLoader::Mesh* pMesh = MeshContainer::GetMeshCPU(GAME_OBJECT_TYPE_UNIT);

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

			Entity* weapon_entity = CreateWeaponEntity(nullptr, GAME_OBJECT_TYPE_FIST, current->getID());
			
			equipment_component.mEquippedWeapon = weapon_entity->getID();
			createComponent<EquipmentComponent>(current->getID(), equipment_component);
		}
	}
}

ecs::Entity* ecs::systems::RoundStartSystem::CreateWeaponEntity(ModelLoader::Mesh* pMesh, GAME_OBJECT_TYPE weaponType, ID ownerEntity)
{
	WeaponComponent		weapon_component;
	TransformComponent	weapon_transform_component;
	MeshComponent		weapon_mesh_component;

	weapon_component.mType = weaponType;
	weapon_component.mOwnerEntity = ownerEntity;
	weapon_mesh_component.mMesh = pMesh;

	switch (weaponType)
	{
	case GAME_OBJECT_TYPE_SWORD:
	{
		weapon_transform_component.scale = XMFLOAT3(0.1f, 0.1f, 0.1f);
		break;
	}
	case GAME_OBJECT_TYPE_FIST:
		weapon_transform_component.scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
		break;
	case GAME_OBJECT_TYPE_PROJECTILE:
		MessageBoxA(NULL, "Projectile weapon not yet implemented.", NULL, MB_YESNO);
		break;
	}

	return createEntity(weapon_mesh_component, weapon_transform_component, weapon_component);
}
///////////////////

ecs::systems::RoundOverSystem::RoundOverSystem()
{
	updateType = EventReader;
	typeFilter.addRequirement(ecs::events::RoundEndEvent::typeID);

	this->mRoundOver = false;
	this->mRoundOverDuration = 0.0f;
}

ecs::systems::RoundOverSystem::~RoundOverSystem()
{
}

void ecs::systems::RoundOverSystem::readEvent(BaseEvent& event, float delta)
{
	if (event.getTypeID() == ecs::events::RoundEndEvent::typeID && !this->mRoundOver)
	{
		int winner = dynamic_cast<ecs::events::RoundEndEvent*>(&event)->winner;

		// Failsafe if the evetn wasnt created correct, -1 is also a draw
		if (winner >= 0)
		{
			ComponentIterator itt = ecs::ECSUser::getComponentsOfType(ecs::components::GameLoopComponent::typeID);
			GameLoopComponent* p_gl;
			while (p_gl = static_cast<GameLoopComponent*>(itt.next()))
			{

				// Check if the winner will sin the game now or not
				if (p_gl->mPlayerPoints[winner] < ROUNDS_TO_WIN - 1)
				{
					p_gl->mPlayerPoints[winner]++;

					cout << "The round winner is Player " << winner << endl;
					// Can be reworked to start prep phase
					this->mRoundOver = true;
					itt = getComponentsOfType<UITextComponent>();
					
					
					UITextComponent* text_comp;
					while (text_comp = (UITextComponent*)itt.next())
					{
						if (text_comp->tag == UITAG::STARTTEXT)
						{
							switch (winner)
							{
							case PLAYER1:
								text_comp->mStrText = "RED won the round!";
								break;
							case PLAYER2:
								text_comp->mStrText = "PURPLE won the round!";
								break;
							case PLAYER3:
								text_comp->mStrText = "BLUE won the round!";
								break;
							case PLAYER4:
								text_comp->mStrText = "GREEN won the round!";
								break;
							default:
								break;
							}
						}
					}
				}
				else
				{
					// What to do when a player has won
					UITextComponent* text_comp;
					itt = getComponentsOfType<UITextComponent>();
					while (text_comp = (UITextComponent*)itt.next())
					{
						if (text_comp->tag == UITAG::STARTTEXT)
						{
							switch (winner)
							{
							case PLAYER1:
								text_comp->mStrText = "RED WON THE GAME!!!!";
								break;
							case PLAYER2:
								text_comp->mStrText = "PURPLE WON THE GAME!!!!";
								break;
							case PLAYER3:
								text_comp->mStrText = "BLUE WON THE GAME!!!!";
								break;
							case PLAYER4:
								text_comp->mStrText = "GREEN WON THE GAME!!!!";
								break;
							default:
								break;
							}
						}
					}
				}
			}
		}
	}

	if (this->mRoundOver)
	{
		this->mRoundOverDuration += delta;

		if (this->mRoundOverDuration > 3.0f)
		{
			ComponentIterator itt;
			// Puts the players into waiting phase
			itt = getComponentsOfType<InputBackendComp>();
			InputBackendComp* p_ib;
			while (p_ib = (InputBackendComp*)itt.next())
			{
				p_ib->backend->changeGamestate(WEBGAMESTATE::PREPPHASE);
			}

			// Remove battlephase and start prephase
			RemoveSystem(systems::BattlephaseSystem::typeID);
			CreateSystem<systems::PrepphaseSystem>(1);

			this->mRoundOver = false;
			this->mRoundOverDuration = 0.0f;
		}
	}
}