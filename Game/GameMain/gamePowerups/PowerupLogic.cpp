#include "PowerupLogic.h"
#include "PowerupEvents.h"
#include "PowerupComponents.h"
#include "../gameUtility/UtilityComponents.h"
#include "../gameAI/AIComponents.h"
#include "../Physics/PhysicsComponents.h"

using namespace DirectX;
using namespace ecs::components;

namespace ecs
{
	namespace systems
	{
		/*
			#################################
			-- PowerupLootCollisionSystem
			#################################
		*/

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

						switch (p_powerup_loot->mPowerupType)
						{
						case GAME_OBJECT_TYPE_POWERUP_HEALTH_PACK:
						{
							events::HealthPackPickupEvent trigger_event;
							trigger_event.affectedUnitId = unit.entity->getID();
							trigger_event.healPercentOfBaseHp = 50.f;
							createEvent(trigger_event);

							removeEntity(powerup.entity->getID());
							break;
						}

						default:
							break;
						}

						

						/*
							Break out from checking any other unit colliding with current powerup,
							since the powerup is now removed.
						*/

						break;
					}
				} // end for units
			} // end for powerups
		}



		/*
			#################################
			-- HealthPackTriggerSystem
			#################################
		*/

		HealthPackTriggerSystem::HealthPackTriggerSystem()
		{
			updateType = EventReader;
			typeFilter.addRequirement(events::HealthPackPickupEvent::typeID);
		}

		HealthPackTriggerSystem::~HealthPackTriggerSystem()
		{
			//
		}

		void HealthPackTriggerSystem::readEvent(BaseEvent& _event, float _delta)
		{
			// Sanity check event type
			if (_event.getTypeID() != events::HealthPackPickupEvent::typeID)
			{
				return;
			}

			/*
				Cast event in order to extract its data.
			*/

			events::HealthPackPickupEvent& r_event = static_cast<events::HealthPackPickupEvent&>(_event);

			// Sanity check unit existance
			if (!getEntity(r_event.affectedUnitId))
			{
				return;
			}

			components::HealthComponent* p_health_comp = getComponentFromKnownEntity<HealthComponent>(r_event.affectedUnitId);

			// Sanity check component existance
			if (!p_health_comp)
			{
				return;
			}

			p_health_comp->mHealth = min(p_health_comp->mHealth + r_event.healPercentOfBaseHp, p_health_comp->mBaseHealth);
		}
	}
}
