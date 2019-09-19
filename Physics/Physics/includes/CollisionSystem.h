#pragma once
#include "ecs.h"
#include "Components.h"
#include "Events.h"

#define SYSTEM(name) struct name : public ecs::ECSSystem<name>

namespace ecs
{
	namespace systems
	{
		/** CollisionEventSystem:
		* Checks collision between bounding spheres 
		* and creates a collision event for when
		* a collision occurs.
		*/
		SYSTEM(CollisionEventSystem)
		{
			CollisionEventSystem();
			~CollisionEventSystem();
			void readEvent(ecs::BaseEvent & _event, float _delta) override;
		};

		/** CollisionHandlingSystem:
		* Receives collision events and... 
		*/
		SYSTEM(CollisionHandlingSystem) {};
	} // systems
} // ecs