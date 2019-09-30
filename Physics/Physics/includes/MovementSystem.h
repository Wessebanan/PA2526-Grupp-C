#pragma once
#include "ecs.h"
#include "PhysicsComponents.h"
#include "PhysicsEvents.h"
#include "UtilityComponents.h"
#include <math.h>
#include "PhysicsHelperFunctions.h"

#define SYSTEM(name) struct name : public ecs::ECSSystem<name>

#define DEFAULT_MOVEMENT_FORCE 500.0f
#define DEFAULT_DECELERATION 10.0f
#define DEFAULT_VELOCITY 100.0f
#define PI 3.14159265358979323846

using namespace PhysicsHelpers;

namespace ecs
{
	namespace systems
	{
		/** Movement systems:
		* When any entity needs to move, the movement systems
		* take care of that. The movement systems assume movement
		* happens primarily in the x-y plane, with movement along
		* the z-axis being side-effects of moving in the x-y plane.
		*/

		// Applies movement changes to moving entities, and assumes
		// no further input.
		SYSTEM(StaticMovementSystem)
		{
			StaticMovementSystem();
			virtual ~StaticMovementSystem();
			void updateEntity(ecs::FilteredEntity & _entityInfo, float _delta) override;
		};
		
		// Applies direction and velocity to entities affected by
		// input events related to movement.
		SYSTEM(StaticMovementUpdateSystem)
		{
			StaticMovementUpdateSystem();
			virtual ~StaticMovementUpdateSystem();
			void readEvent(ecs::BaseEvent& _event, float _delta) override;
		};

		SYSTEM(DynamicMovementSystem)
		{
			DynamicMovementSystem();
			~DynamicMovementSystem();
			void updateEntity(ecs::FilteredEntity & _entityInfo, float _delta) override;
			void onEvent(TypeID _typeID, ecs::BaseEvent * _event) override;
		};

	} // systems
} // ecs