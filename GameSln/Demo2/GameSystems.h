#pragma once
#include <ecsSystemIncludes.h>
#include <UtilityComponents.h>
#include <AIComponents.h>
#include <InputComponents.h>
#include "GameComponents.h"



namespace ecs
{
	namespace systems
	{
		class GetTileSystem : public ECSSystem<GetTileSystem>
		{
		public:
			GetTileSystem()
			{
				updateType = EntityUpdate;
				typeFilter.addRequirement(components::TileComponent::typeID);
				typeFilter.addRequirement(components::TransformComponent::typeID);
			}
			virtual ~GetTileSystem() {}
			void updateEntity(FilteredEntity& entity, float delta) override
			{
				int test = 0;
				components::TransformComponent* pTc = entity.getComponent<components::TransformComponent>();



			}
		};

		class GameOverSystem : public ECSSystem<GameOverSystem>
		{
		public:
			GameOverSystem()
			{
				updateType = EntityUpdate;
				typeFilter.addRequirement(components::GameStateComponent::typeID);
			}
			virtual ~GameOverSystem() {}
			void updateEntity(FilteredEntity& entity, float delta) override
			{
				int test = 0;
				//components::KeyboardComponent* kbComp = entity.getComponent<components::KeyboardComponent>();
				components::GameStateComponent* gsComp = entity.getComponent<components::GameStateComponent>();
				
				components::KeyboardComponent* kbComp = ((components::KeyboardComponent*)this->getComponentsOfType(components::KeyboardComponent::typeID).next());

				if (kbComp->R)
				{
					gsComp->gameState = 666;
				}


			}
		};

	}
}
///////////////////////////////////////////////////////////////////////////////////
//System Template
//Use this template when creating a new system for the camera.
//////////////////////////////////////////////////////////////////////////////////
//class testSystem : public ECSSystem<testSystem>
//{
//public:
//	testSystem()
//	{
//		updateType = EntityUpdate;
//		componentFilter.addRequirement(components::CameraComponent::typeID);
//		//eventFilter.addRequirement
//		//subscribeEventCreation(

//		//// Entity ID -> TilePointer
//		//std::map<ID, int> tiles;

//		//for (int i = 0; i < 10; i++)
//		//{
//		//	// Skapar
//		//	tiles[i] = i+1;
//		//}
//		//
//		//std::map<ID, int>::iterator it;
//		//for (it = tiles.begin(); it != tiles.end(); it++)
//		//{
//		//	// it.first : ID
//		//	// it.second : int
//		//}
//	}
//	virtual ~testSystem() {}

//	// PICK ONE
//	//void updateEntity(FilteredEntity& entity, float delta) override;
//	//void updateMultipleEntities(EntityIterator& entities, float delta) override;
//	//void readEvent(BaseEvent& event, float delta) override;

//	// OPTIONAL
//	//void onEvent(TypeID eventType, BaseEvent *pEvent) override;
//};