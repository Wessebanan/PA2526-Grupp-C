#pragma once
#include "ecs.h"

#include "PhysicsComponents.h"
#include "PhysicsEvents.h"
#include "PhysicsHelperFunctions.h"
#include "CollisionFunctions.h"
#include "QuadTree.h"

#include "../gameAI/AIComponents.h"

#include "../gameUtility/UtilityComponents.h"
#include "../gameUtility/UtilityEcsFunctions.h"
#include "GridFunctions.h"
#include "GridProp.h"

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
			EntityIterator mTiles;
			TransformComponent** mppTileTransforms = nullptr;
			unsigned int mTileCount = 0;
			bool mInit = false;
		};

		/** FillQuadTreeSystem:
		* Each update this system fills the quadtree with QuadTreeObjects containing
		* information needed to check for collisions. The tree needs to be manually cleared
		* between each update.
		*/
		SYSTEM(FillQuadTreeSystem)
		{
			FillQuadTreeSystem();
			~FillQuadTreeSystem();
			void updateEntity(FilteredEntity & _entityInfo, float _delta) override;
		};

		/** EmptyQuadTreeSystem:
		* Empties the quad tree every frame.
		*/
		SYSTEM(EmptyQuadTreeSystem)
		{
			EmptyQuadTreeSystem();
			~EmptyQuadTreeSystem();
			void updateEntity(FilteredEntity & _entityInfo, float _delta) override;
		};

		SYSTEM(InitQuadTreeSystem)
		{
			InitQuadTreeSystem();
			~InitQuadTreeSystem();
			void onEvent(TypeID _typeId, ecs::BaseEvent * _event) override;
		};
	} // systems
} // ecs