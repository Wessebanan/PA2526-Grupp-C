#pragma once
#include "ecsSystemIncludes.h"
#include "CameraComponents.h"
#include "UtilityComponents.h"
#include "../Input/InitInputComponents.h"
#include "GlobalsCamera.h"
#include "GridProp.h"
#include "../../..//Physics/Physics/includes/PhysicsHelperFunctions.h"
#include <iostream>

namespace ecs
{
	namespace systems
	{
		class FreelookCameraSystem : public ECSSystem<FreelookCameraSystem>
		{
		public:
			FreelookCameraSystem();
			virtual ~FreelookCameraSystem();
			void updateEntity(FilteredEntity& entity, float delta) override;
		};


		class DynamicCameraSystem : public ECSSystem<DynamicCameraSystem>
		{
		public:
			DynamicCameraSystem();
			virtual ~DynamicCameraSystem();
			void updateEntity(FilteredEntity& entity, float delta) override;
		private:
			ID mCamEntityId;
			const float mT = 0.01f;
			const float mPercent = 0.02;
			inline void GetTargetPosition(DirectX::XMVECTOR& rTarget, DirectX::XMVECTOR& rLookAt);
			inline void CheckCameraAngle(DirectX::XMVECTOR& cameraPos, DirectX::XMVECTOR& targetPos);

		};

		class ArmyZoomCameraSystem : public ECSSystem<ArmyZoomCameraSystem>
		{
		public:
			ArmyZoomCameraSystem();
			virtual ~ArmyZoomCameraSystem();
			void updateEntity(FilteredEntity& entity, float delta) override;
		private:
			int mCurrentArmyTarget;
			bool mZoomIn;
			bool mCameraInPosition;
		};

		class OverlookCameraSystem : public ECSSystem<OverlookCameraSystem>
		{
		public:
			OverlookCameraSystem();
			virtual ~OverlookCameraSystem();
			void updateEntity(FilteredEntity& entity, float delta) override;
		private:
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