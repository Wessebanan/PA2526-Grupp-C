#include "WeaponSpawner.h"
#include "WeaponEvents.h"
#include "../gameUtility/UtilityComponents.h"
#include "../Physics/PhysicsComponents.h"
#include "../MeshContainer/MeshContainer.h"

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
			components::MeshComponent weapon_mesh_comp;

			weapon_transform_comp.position = tile_position;
			weapon_transform_comp.position.y += 0.2f; // Default position right above the tile. No rotation per default. Default default.

			weapon_comp.mType = r_spawn_event.weaponType;
			
			weapon_mesh_comp.mMesh = MeshContainer::GetMeshCPU(r_spawn_event.weaponType);

			/*
				Check what type of weapon to spawn, and set correct position offset from
				tile and color of the weapon type.

				If a type isn't specified in the switch case below, that weapon type will
				just spawn above the tile. Default color is black.
			*/

			switch (r_spawn_event.weaponType)
			{
			case GAME_OBJECT_TYPE_WEAPON_SWORD:
			{
				//weapon_transform_comp.position.y += 0.3f;
				weapon_transform_comp.scale = XMFLOAT3(0.1f, 0.1f, 0.1f);
				break;
			}


			default:
			{
				weapon_color_comp.red = weapon_color_comp.green = weapon_color_comp.blue = 0;
				break;
			}
			}

			createEntity(weapon_mesh_comp, weapon_transform_comp, weapon_color_comp, weapon_comp);
		}
	}
}

