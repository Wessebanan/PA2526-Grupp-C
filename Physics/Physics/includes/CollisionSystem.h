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
			void onEvent(TypeID _typeID, ecs::BaseEvent * _event) override;
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