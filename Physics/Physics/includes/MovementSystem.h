#pragma once
//#include "ecs.h"
#include "Components.h"
#include "Events.h"
#include "UtilityComponents.h"
#include <math.h>

#define SYSTEM(name) struct name : public ecs::ECSSystem<name>

#define DEFAULT_DECELERATION 10.0f
#define DEFAULT_VELOCITY 100.0f
#define PI 3.14159265

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

		// Updates static movement for statically moving entities.
		SYSTEM(StaticMovementSystem)
		{
			StaticMovementSystem();
			virtual ~StaticMovementSystem();
			void updateEntity(ecs::FilteredEntity & _entityInfo, float _delta) override;
		};
		
		// Handles input for static movement.
		SYSTEM(StaticMovementUpdateSystem)
		{
			StaticMovementUpdateSystem();
			virtual ~StaticMovementUpdateSystem();
			void readEvent(ecs::BaseEvent & _event, float _delta) override;
		
		};
	}
}