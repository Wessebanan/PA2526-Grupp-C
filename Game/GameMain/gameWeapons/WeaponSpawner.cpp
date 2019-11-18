#include "WeaponSpawner.h"
#include "WeaponEvents.h"
#include "../gameUtility/UtilityComponents.h"
#include "../Physics/PhysicsComponents.h"
#include "../MeshContainer/MeshContainer.h"
#include "../gameGraphics/ParticleECSComponents.h"

using namespace DirectX;

namespace ecs
{
	namespace systems
	{
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

			weapon_transform_comp.position = tile_position;
			weapon_transform_comp.position.y += 0.2f; // Default position right above the tile. No rotation per default. Default default.

			weapon_comp.mType = r_spawn_event.weaponType;			

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

			createEntity(weapon_transform_comp, weapon_color_comp, weapon_comp);

			/* Spawn Smoke Emitter At Sword Spawn */
			components::ParticleSpawnerComponent spawner;
			components::SmokeSpawnerComponent smoke;

			spawner.StartPosition = weapon_transform_comp.position;
			spawner.SpawnFrequency = 0.005f;
			spawner.TimerSinceLastSpawn = 0.0f;
			spawner.LifeDuration = 1.0f;

			smoke.InitialVelocity = 8.0f;
			smoke.SpawnCount = 100;

			createEntity(spawner, smoke);
		}
	}
}

