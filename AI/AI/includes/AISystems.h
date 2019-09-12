#pragma once

#include <map>
#include "ecsSystemIncludes.h"

#include "AIComponents.h"

namespace ecs
{
	namespace systems
	{
		class testSystem : public ECSSystem<testSystem>
		{
		public:
			testSystem()
			{
				updateType = EntityUpdate;
				componentFilter.addRequirement(components::TileComponent::typeID);
				//eventFilter.addRequirement
				//subscribeEventCreation(

				//// Entity ID -> TilePointer
				//std::map<ID, int> tiles;

				//for (int i = 0; i < 10; i++)
				//{
				//	// Skapar
				//	tiles[i] = i+1;
				//}
				//
				//std::map<ID, int>::iterator it;
				//for (it = tiles.begin(); it != tiles.end(); it++)
				//{
				//	// it.first : ID
				//	// it.second : int
				//}
			}
			virtual ~testSystem() {}

			// PICK ONE
			//void updateEntity(FilteredEntity& entity, float delta) override;
			//void updateMultipleEntities(EntityIterator& entities, float delta) override;
			//void readEvent(BaseEvent& event, float delta) override;

			// OPTIONAL
			//void onEvent(TypeID eventType, BaseEvent *pEvent) override;
		};

		/*
			A system that prints every tiles centerposition in the order it was 
			created. Only used for debugging purposes.
		*/
		class TilePrintSystem : public ECSSystem<TilePrintSystem>
		{
		public:
			TilePrintSystem()
			{
				updateType = EntityUpdate;
				componentFilter.addRequirement(components::TileComponent::typeID);
				componentFilter.addRequirement(components::PositionComponent::typeID);
			}
			virtual ~TilePrintSystem() {}

			//Update function that prints the center position of every tile in the order they 
			//were created.
			void updateEntity(FilteredEntity& entity, float delta) override
			{
				PositionComponent* pPc = entity.getComponent<PositionComponent>();
				std::cout << "Entity: " << pPc->getEntityID() << " Position: " << pPc->x << " , " << pPc->y << " , " << pPc->z << std::endl;
			}
		};
	}
}