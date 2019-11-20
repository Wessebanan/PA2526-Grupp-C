#include "PowerupLogic.h"
#include "PowerupComponents.h"
#include "../gameUtility/UtilityComponents.h"
#include "../gameAI/AIComponents.h"

#include "../gameUtility/UtilityGraphics.h"

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
							Handle pickup
						*/

						//removeEntity(powerup.entity->getID());

						p_powerup_loot->mColor = PACK(0, 255, 255, 255);

						break;
					}
				}
			}
		}
	}
}
