#include "PowerupLogic.h"
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

			TransformComponent* p_unit_transf;
			TransformComponent* p_powerup_transf;
			PowerupLootComponent* p_powerup_loot;
			for (FilteredEntity powerup : rPowerups.entities)
			{
				p_powerup_transf = powerup.getComponent<TransformComponent>();

				/*
					Iterate all units and handle all that are within pickup range.
				*/

				for (FilteredEntity unit : units.entities)
				{
					p_unit_transf = unit.getComponent<TransformComponent>();

					if (XMVectorGetX(XMLoadFloat3(&p_powerup_transf->position) - XMLoadFloat3(&p_unit_transf->position)) <= PICKUP_RANGE)
					{
						p_powerup_loot = powerup.getComponent<PowerupLootComponent>();

						/*
							Handle pickup
						*/

						removeEntity(powerup.entity->getID());

						break;
					}
				}
			}
		}
	}
}
