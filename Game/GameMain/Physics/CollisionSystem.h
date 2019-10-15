#pragma once
#include "ecs.h"

#include "PhysicsComponents.h"
#include "PhysicsEvents.h"
#include "PhysicsHelperFunctions.h"
#include "CollisionFunctions.h"

#include "../gameAI/AIComponents.h"

#include "../gameUtility/UtilityComponents.h"
#include "../gameUtility/UtilityEcsFunctions.h"

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
		private:
			EntityIterator tiles;
			TransformComponent** tile_transforms = nullptr;
			unsigned int tile_count = 0;
			bool init = false;
		};

	} // systems
} // ecs