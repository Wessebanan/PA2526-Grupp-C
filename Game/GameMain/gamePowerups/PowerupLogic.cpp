#include "PowerupLogic.h"
#include "PowerupEvents.h"
#include "PowerupComponents.h"
#include "../gameUtility/UtilityComponents.h"
#include "../gameAI/AIComponents.h"

using namespace DirectX;
using namespace ecs::components;

namespace ecs
{
	namespace systems
	{
		PowerupLootCollisionSystem::PowerupLootCollisionSystem()
		{
			updateType = MultiEntityUpdate;
			typeFilter.addRequirement(PowerupLootComponent::typeID);
			typeFilter.addRequirement(TransformComponent::typeID);
		}

		PowerupLootCollisionSystem::~PowerupLootCollisionSystem()
		{
			//
		}

		void PowerupLootCollisionSystem::updateMultipleEntities(EntityIterator& rPowerups, float delta)
		{
			/*
				Fetch all units, happens once per frame.
			*/

			TypeFilter unitFilter;
			unitFilter.addRequirement(UnitComponent::typeID);
			unitFilter.addRequirement(TransformComponent::typeID);
			EntityIterator units = getEntitiesByFilter(unitFilter);


			/*
				Iterate all powerups
			*/

			XMVECTOR unit_xz_location;
			XMVECTOR powerup_xz_location;
			PowerupLootComponent* p_powerup_loot;
			for (FilteredEntity powerup : rPowerups.entities)
			{
				powerup_xz_location = XMLoadFloat3(&powerup.getComponent<TransformComponent>()->position);
				XMVectorSetY(powerup_xz_location, 0.f);

				/*
					Iterate all units and handle all that are within pickup range.
				*/

				for (FilteredEntity unit : units.entities)
				{
					// We can safely expect units to have a transform component.
					unit_xz_location = XMLoadFloat3(&unit.getComponent<TransformComponent>()->position);
					XMVectorSetY(unit_xz_location, 0.f);

					if (XMVectorGetX(XMVector3Length(powerup_xz_location - unit_xz_location)) <= PICKUP_RANGE)
					{
						p_powerup_loot = powerup.getComponent<PowerupLootComponent>();

						/*
							Create powerup trigger event and remove powerup loot entity.
						*/

						events::PowerupTrigger trigger_event;
						trigger_event.powerupType = p_powerup_loot->mPowerupType;
						trigger_event.affectedUnitId = unit.entity->getID();
						createEvent(trigger_event);

						removeEntity(powerup.entity->getID());

						/*
							Break out from checking any other unit colliding with current powerup,
							since the powerup is now removed.
						*/

						break;
					}
				} // end for units
			} // end for powerups
		}
	}
}
