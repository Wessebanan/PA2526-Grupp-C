#include "WeaponSpawner.h"
#include "WeaponEvents.h"
#include "../gameUtility/UtilityComponents.h"
#include "../Physics/PhysicsComponents.h"
#include "../MeshContainer/MeshContainer.h"
#include "../gameGraphics/ParticleECSComponents.h"

#include "../gameAI/AIComponents.h" // TileComponent

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
			components::MeshComponent weapon_mesh_comp;

			weapon_transform_comp.position = tile_position;
			weapon_transform_comp.position.y += 0.2f; // Default position right above the tile. No rotation per default. Default default.

			weapon_comp.mType = r_spawn_event.weaponType;
			
			weapon_mesh_comp.mMesh = MeshContainer::GetMeshCPU(r_spawn_event.weaponType);

			/*
				Check weapon type and set specific values for that type.
			*/

			switch (r_spawn_event.weaponType)
			{
			case GAME_OBJECT_TYPE_WEAPON_SWORD:
			{
				/*
					"Spear sword to the ground."
					- Emil 2019
				*/

				weapon_transform_comp.scale = XMFLOAT3(0.1f, 0.1f, 0.1f);
				weapon_transform_comp.position.y += 0.7f;
				weapon_transform_comp.rotation.x -= 3.14f * 0.4f;
				weapon_transform_comp.rotation.y += 3.14f * 0.3f;

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

			createEntity(weapon_mesh_comp, weapon_transform_comp, weapon_color_comp, weapon_comp);

			/* Spawn Smoke Emitter At Sword Spawn */
			components::ParticleSpawnerComponent spawner;
			components::SmokeSpawnerComponent smoke;

			spawner.StartPosition = tile_position;
			spawner.SpawnFrequency = 0.005f;
			spawner.TimerSinceLastSpawn = 0.0f;
			spawner.LifeDuration = 1.0f;

			smoke.InitialVelocity = 8.0f;
			smoke.SpawnCount = 100;

			createEntity(spawner, smoke);
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

				if (p_tile_comp->tileType == WATER)
				{
					continue;
				}

				tile_position = XMLoadFloat3(&tile.getComponent<components::TransformComponent>()->position);
				XMVectorSetY(tile_position, 0.f);

				if (XMVectorGetX(XMVector3Length(center - tile_position)) < SPAWN_RADIUS)
				{
					mPossibleTileIds.push_back(tile.entity->getID());
				}
			}
		}

		ID MasterWeaponSpawner::FindSpawnTile()
		{
			int random_index = rand() % (int)mPossibleTileIds.size();

			GridProp::GetInstance()->mLootTiles.push_back(mPossibleTileIds[random_index]);
			
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
	}
}
