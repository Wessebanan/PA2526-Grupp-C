#pragma once
#include "ecs.h"
#include "Components.h"
#include "Events.h"
#include "ecsEvent.h"

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

		SYSTEM(ObjectBoundingVolumeInitSystem)
		{
			ObjectBoundingVolumeInitSystem();
			~ObjectBoundingVolumeInitSystem();
			void onEvent(TypeID _typeID, ecs::BaseEvent * _event) override;
		};

		/** CollisionHandlingSystem:
		* Receives collision events and... 
		*/
		SYSTEM(ObjectCollisionSystem) 
		{
			ObjectCollisionSystem();
			~ObjectCollisionSystem();
		};

		/** GroundCollisionComponentInitSystem:
		* Creates the OBB that is used for ground
		* collision whenever a ground collision
		* component is created.
		*/
		SYSTEM(GroundCollisionComponentInitSystem)
		{
			GroundCollisionComponentInitSystem();
			~GroundCollisionComponentInitSystem();
			void onEvent(TypeID _typeID, ecs::BaseEvent * _event) override;
		};

	} // systems
} // ecs