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

		/** GroundCollisionComponentInitSystem:
		* Whenever a ground collision component is
		* created, this system calculates the lowest
		* point of the mesh to compare against the ground.
		*/
		SYSTEM(GroundCollisionComponentInitSystem)
		{
			GroundCollisionComponentInitSystem();
			~GroundCollisionComponentInitSystem();
			void readEvent(ecs::BaseEvent & _event, float _delta) override;
		};

	} // systems
} // ecs