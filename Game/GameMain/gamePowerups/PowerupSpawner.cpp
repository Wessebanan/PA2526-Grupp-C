#include "PowerupSpawner.h"
#include "PowerupEvents.h"
#include "PowerupComponents.h"
#include "../gameUtility/UtilityComponents.h"

#include "../gameUtility/UtilityGraphics.h"

namespace ecs
{
	namespace systems
	{
		PowerupSpawner::PowerupSpawner()
		{
			updateType = EventReader;
			typeFilter.addRequirement(events::SpawnPowerupEvent::typeID);
		}

		PowerupSpawner::~PowerupSpawner()
		{
			//
		}

		void PowerupSpawner::readEvent(BaseEvent& _event, float _delta)
		{
			events::SpawnPowerupEvent& r_event = static_cast<events::SpawnPowerupEvent&>(_event);

			components::TransformComponent* p_tile_transform_comp =
				getComponentFromKnownEntity<components::TransformComponent>(r_event.spawnTileId);

			/*
				Sanity check returning component pointer. If not existing, return early.
			*/

			if (!p_tile_transform_comp)
			{
				return;
			}

			/*
				Create the general description components for powerup loot
			*/

			components::TransformComponent transform_comp;
			components::PowerupLootComponent loot_comp;

			/*
				Copy position of spawn tile, and increase y-value to make
				spawn be above tile.
			*/

			transform_comp.position = p_tile_transform_comp->position;
			transform_comp.position.y += 0.7f;

			transform_comp.scale = { 0.5f, 0.03f, 0.5f };
			transform_comp.rotation.x = 3.14f / 2.f;

			loot_comp.mObjectType = r_event.powerupType;

			/*
				Create powerup loot entity. Then, add type specific components to it.
			*/

			Entity* p_powerup_loot_entity = createEntity(transform_comp, loot_comp);

			switch (r_event.powerupType)
			{
			case GAME_OBJECT_TYPE_POWERUP_HEALTH_PACK:
			{
				components::HealthPackComponent health_pack_comp;
				health_pack_comp.mHealAmount = 100.f;
				createComponent(p_powerup_loot_entity->getID(), health_pack_comp);

				getComponentFromKnownEntity<components::PowerupLootComponent>(p_powerup_loot_entity->getID())->mColor = PACK(0, 255, 0, 255);
				break;
			}
			}
		}
	}
}