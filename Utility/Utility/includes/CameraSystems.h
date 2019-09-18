#pragma once
#include <ecsSystemIncludes.h>
#include <CameraComponents.h>
#include <UtilityComponents.h>

namespace ecs
{
	namespace systems
	{
		class UpdateCameraSystem : public ECSSystem<UpdateCameraSystem>
		{
		public:
			UpdateCameraSystem()
			{
				updateType = EntityUpdate;
				typeFilter.addRequirement(components::CameraComponent::typeID);
				typeFilter.addRequirement(components::TransformComponent::typeID);
			}
			virtual ~UpdateCameraSystem() {}
			void updateEntity(FilteredEntity& entity, float delta) override
			{
				/* Implement in the "Controll camera with input"-task */
				int test = 0;
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