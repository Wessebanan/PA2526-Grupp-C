#include "WeaponSpawner.h"
#include "WeaponEvents.h"
#include "../gameUtility/UtilityComponents.h"
#include "../Physics/PhysicsComponents.h"
#include "../MeshContainer/MeshContainer.h"
#include "../gameGraphics/ParticleECSComponents.h"

#include "../gameAI/AIComponents.h" // TileComponent
#include "../gameAudio/AudioECSEvents.h"

#include "GridProp.h"

using namespace DirectX;

namespace ecs
{
	namespace systems
	{
		/*
			##########################################################
			################## WEAPON SPAWNER ########################
			##########################################################		
		*/

		WeaponSpawner::WeaponSpawner()
		{
			updateType = EventReader;
			typeFilter.addRequirement(events::SpawnWeaponEvent::typeID);
		}

		WeaponSpawner::~WeaponSpawner()
		{
			//
		}

		void WeaponSpawner::readEvent(BaseEvent& _event, float _delta)
		{
			// Sanity check, may not be necessary.
			if (_event.getTypeID() != events::SpawnWeaponEvent::typeID)
			{
				return;
			}

			// Cast event in order to read weapon type
			events::SpawnWeaponEvent& r_spawn_event = static_cast<events::SpawnWeaponEvent&>(_event);

			// Fetch position of tile to spawn on
			XMFLOAT3 tile_position = getComponentFromKnownEntity<components::TransformComponent>(r_spawn_event.spawnTileId)->position;

			components::TransformComponent weapon_transform_comp;
			components::WeaponComponent weapon_comp;
			components::ColorComponent weapon_color_comp;
			components::DynamicMovementComponent weapon_dyn_move_comp;
			components::FallingWeaponComponent falling_comp;			

			weapon_transform_comp.position = tile_position;
			weapon_transform_comp.position.y += 20.2f; // Default position right above the tile up in the air. No rotation per default. Default default.

			//weapon_comp.mType = r_spawn_event.weaponType = GAME_OBJECT_TYPE_WEAPON_BOMB;			
			weapon_comp.mType = r_spawn_event.weaponType;			

			/*
				Check weapon type and set specific values for that type.
			*/

			
			float weapon_offset_y = 0.f; // How far above the tile the weapon should be at pick-up stage 
			weapon_transform_comp.scale = XMFLOAT3(0.1f, 0.1f, 0.1f); // To have same scale as when picked up

			switch (r_spawn_event.weaponType)
			{
			case GAME_OBJECT_TYPE_WEAPON_SWORD:
			case GAME_OBJECT_TYPE_WEAPON_HAMMER:
			{
				/*
					"Spear sword to the ground."
					- Emil 2019
				*/

				//weapon_transform_comp.scale = XMFLOAT3(0.1f, 0.1f, 0.1f);
				weapon_offset_y = 1.1f;

				// Set Rotation when at pick-up stage
				weapon_transform_comp.rotation.x -= 3.14f * 0.4f;
				weapon_transform_comp.rotation.y += 3.14f * 0.3f;
				

				weapon_color_comp.red = 200;
				weapon_color_comp.green = 130;
				weapon_color_comp.blue = 20;

				break;
			}

			case GAME_OBJECT_TYPE_WEAPON_BOMB:
			{
				weapon_offset_y = 0.05f;

				weapon_color_comp.red = 200;
				weapon_color_comp.green = 130;
				weapon_color_comp.blue = 20;

				break;
			}


			default:
			{
				weapon_color_comp.red = weapon_color_comp.green = weapon_color_comp.blue = 0;
				break;
			}
			}

			falling_comp.rotation = weapon_transform_comp.rotation;
			falling_comp.mTileId = r_spawn_event.spawnTileId;
			falling_comp.mPosYOffset = weapon_offset_y;
			falling_comp.mPosY = tile_position.y;

			/*CAREPACKAGE ENTITY*/
			/*MIGHT WANT IT BACK LATER*/
			//components::TransformComponent cp_transform;
			//cp_transform.position = weapon_transform_comp.position;
			//cp_transform.scale = { 1.0f, 1.0f, 1.0f };
			//components::DefaultRenderObjectComponent dro_comp;
			//dro_comp.type = GAME_OBJECT_TYPE_BOX;
			//components::ColorComponent color_comp;
			//color_comp.red = 101.0f;
			//color_comp.green = 67.0f;
			//color_comp.blue = 33.0f;
			//components::DynamicMovementComponent dyn_move_comp;
			//Entity* carepackage_entity = ECSUser::createEntity(cp_transform, dro_comp, color_comp, dyn_move_comp);
			//falling_comp.mCarepackageId = carepackage_entity->getID();
			
			//Create the Weapon entity.
			ecs::BaseComponent* components[] =
			{
				&weapon_transform_comp, 
				&weapon_color_comp, 
				&weapon_comp, 
				&weapon_dyn_move_comp, 
				&falling_comp, 
			};
			ecs::ComponentList list;
			list.initialInfo = components;
			list.componentCount = 5;
				
			createEntity(list);
		}



		/*
			##########################################################
			############### MASTER WEAPON SPAWNER ####################
			##########################################################
		*/

		MasterWeaponSpawner::MasterWeaponSpawner()
		{
			updateType = Actor;
			ResetSpawnTimer();
		}

		MasterWeaponSpawner::~MasterWeaponSpawner()
		{
		}

		void MasterWeaponSpawner::act(float _delta)
		{
			mSpawnTimer -= _delta;

			if (mSpawnTimer <= 0.f)
			{
				events::SpawnWeaponEvent spawn_event;
				spawn_event.weaponType = GetRandomWeaponType();
				spawn_event.spawnTileId = FindSpawnTile();
				createEvent(spawn_event);

				ResetSpawnTimer();
			}
		}

		void MasterWeaponSpawner::Initialize()
		{
			/*
				We want weapons to spawn on the inner-island; not not the islets. To do this, we find and
				store all the tiles we are interested in for spawning and randomizes a spawn on these when
				spawn cooldown has reached zero.

				Since tiles never change, we can store the interesting tiles early so we don't have to fetch
				all tiles every update.
			*/

			/*
				Fetch center tile position
			*/

			XMVECTOR center;

			{
				int2 grid_size = GridProp::GetInstance()->GetSize();
				ID center_tile_id = GridProp::GetInstance()->mGrid[(int)(floor(grid_size.y / 2.f))][(int)(floor(grid_size.x / 2.f))].Id;
				center = XMLoadFloat3(&getComponentFromKnownEntity<TransformComponent>(center_tile_id)->position);
				XMVectorSetY(center, 0.f);
			}

			/*
				Iterate all tiles and store all valid ones (not water and within radius).
			*/

			TypeFilter tile_filter;
			tile_filter.addRequirement(components::TileComponent::typeID);
			tile_filter.addRequirement(components::TransformComponent::typeID);
			EntityIterator tiles = getEntitiesByFilter(tile_filter);

			XMVECTOR tile_position;
			components::TileComponent* p_tile_comp;
			for (FilteredEntity& tile : tiles.entities)
			{
				p_tile_comp = tile.getComponent<components::TileComponent>();

				/*
					Skip water tiles
				*/
			
				if (p_tile_comp->tileType == WATER || p_tile_comp->impassable)
				{
					continue;
				}

				tile_position = XMLoadFloat3(&tile.getComponent<components::TransformComponent>()->position);
				XMVectorSetY(tile_position, 0.f);

				float distance_from_center = XMVectorGetX(XMVector3Length(center - tile_position));
				if (distance_from_center >= SPAWN_INNER_RADIUS &&
					distance_from_center <= SPAWN_OUTER_RADIUS)
				{
					mPossibleTileIds.push_back(tile.entity->getID());
				}
			}
		}

		ID MasterWeaponSpawner::FindSpawnTile()
		{
			bool tile_found = false;
			int random_index;
			GridProp* p_gp = GridProp::GetInstance();
			int tries = 0;
			while (!tile_found && tries < 20)
			{
				random_index = rand() % (int)mPossibleTileIds.size();
				tile_found = true;
				for (int i = 0; i < p_gp->mLootTiles.size(); i++)
				{
					if (p_gp->mLootTiles[i] == mPossibleTileIds[random_index])
					{
						tile_found = false;
						tries++;
					}
				}
			}
			p_gp->mLootTiles.push_back(random_index);
			return mPossibleTileIds[random_index];
		}

		GAME_OBJECT_TYPE MasterWeaponSpawner::GetRandomWeaponType()
		{
			/*
				Randomizes a weapon type from GAME_OBJECT_TYPE_WEAPON list.
			*/

			return (GAME_OBJECT_TYPE_WEAPON_OFFSET_TAG + 1) + rand() % WEAPON_TYPE_COUNT;
		}

		void MasterWeaponSpawner::ResetSpawnTimer()
		{
			mSpawnTimer = COOLDOWN_BASE_TIME + ((rand() % COOLDOWN_TIME_VARIANCE + 1) - COOLDOWN_TIME_VARIANCE / 2.f);
		}

		/*
		##########################################################
		############### FALLING WEAPON SYSTEM ####################
		##########################################################
		*/

		FallingWeaponSystem::FallingWeaponSystem()
		{
			updateType = EntityUpdate;
			typeFilter.addRequirement(FallingWeaponComponent::typeID);
			typeFilter.addRequirement(TransformComponent::typeID);
		}

		FallingWeaponSystem::~FallingWeaponSystem()
		{

		}

		void FallingWeaponSystem::updateEntity(FilteredEntity& entity, float delta)
		{
			TransformComponent* p_weapon_transform = ECSUser::getComponentFromKnownEntity<TransformComponent>(entity.entity->getID());
			FallingWeaponComponent* p_falling_weapon = ECSUser::getComponentFromKnownEntity<FallingWeaponComponent>(entity.entity->getID());
			//Carepackage stuff.
			//TransformComponent* p_carepackage_transform = ECSUser::getComponentFromKnownEntity<TransformComponent>(p_falling_weapon->mCarepackageId);

			
			if (p_weapon_transform->position.y < p_falling_weapon->mPosY + p_falling_weapon->mPosYOffset)
			{
				//Set the tile to a loot tile so that units can loot the weapon
				//GridProp::GetInstance()->mLootTiles.push_back(p_falling_weapon->mTileId);
				//Set the y-position and the rotation of the weapon so that it sticks to the ground.
				p_weapon_transform->position.y = p_falling_weapon->mPosY + p_falling_weapon->mPosYOffset;
				p_weapon_transform->rotation = p_falling_weapon->rotation;
				//Remove the movement component and the falling component from the weapon
				ECSUser::removeComponent(entity.entity->getID(), DynamicMovementComponent::typeID);
				ECSUser::removeComponent(entity.entity->getID(), FallingWeaponComponent::typeID);
				//Remove the carepackage
				//ECSUser::removeEntity(p_falling_weapon->mCarepackageId);

				/* Spawn Smoke Emitter At Sword Spawn */
				components::ParticleSpawnerComponent spawner;
				components::SmokeSpawnerComponent smoke;

				spawner.StartPosition = p_weapon_transform->position;
				spawner.StartPosition.y = p_falling_weapon->mPosY;
				spawner.SpawnFrequency = 0.005f;
				spawner.TimerSinceLastSpawn = 0.0f;
				spawner.LifeDuration = 1.0f;

				smoke.InitialVelocity = 8.0f;
				smoke.SpawnCount = 100;

				createEntity(spawner, smoke);

				events::PlaySound m_event;
				m_event.audioName = AudioName::SOUND_itemland;
				createEvent(m_event);
			}
			else
			{
				//Rotate the weapon while falling.
				p_weapon_transform->rotation.x += 10.0f * delta;
				p_weapon_transform->rotation.z += 10.0f * delta;

				//Rotate carepackage, might want it later.
				//p_carepackage_transform->rotation.x += 0.2f;
			}
		}
	}
}

