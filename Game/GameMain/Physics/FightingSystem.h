#pragma once
#include "ecs.h"

#include "PhysicsComponents.h"
#include "PhysicsEvents.h"
#include "PhysicsHelperFunctions.h"
#include "..//gameUtility/UtilityComponents.h"
#include "..//gameUtility/UtilityEcsFunctions.h"
#define SYSTEM(name) struct name : public ecs::ECSSystem<name>

namespace ecs
{
	namespace systems
	{
		SYSTEM(WeaponInitSystem)
		{
			WeaponInitSystem();
			~WeaponInitSystem();
			void onEvent(TypeID _typeID, ecs::BaseEvent * _event) override;
		};

		SYSTEM(DamageSystem)
		{
			DamageSystem();
			~DamageSystem();
			void updateEntity(FilteredEntity & _entityInfo, float _delta) override;
		};
	} // systems
} // ecs
