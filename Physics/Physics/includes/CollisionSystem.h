#pragma once
#include "ecs.h"
#include "PhysicsComponents.h"
#include "PhysicsEvents.h"
#include "ecsEvent.h"
#include "AIComponents.h"
#include "UtilityComponents.h"
#include "UtilityFunctions.h"
#include "PhysicsHelperFunctions.h"

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

		/** GroundCollisionSystem:
		* Each update this system checks each
		* entity with a ground collision component
		* against the tile it is on (nearest center
		* position right now).
		*/
		SYSTEM(GroundCollisionSystem)
		{
			GroundCollisionSystem();
			~GroundCollisionSystem();
			void updateEntity(FilteredEntity & _entityInfo, float _delta) override;
		};

	} // systems
} // ecs