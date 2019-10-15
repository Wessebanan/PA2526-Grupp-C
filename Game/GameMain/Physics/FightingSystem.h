#pragma once
#include "ecs.h"

#include "PhysicsComponents.h"
#include "PhysicsEvents.h"
#include "PhysicsHelperFunctions.h"

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

		//SYSTEM(DamageSystem){};
	} // systems
} // ecs
