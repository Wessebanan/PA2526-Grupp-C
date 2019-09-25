#pragma once
//#include "InitInputHandler.h"
#include "ecs.h"
#include "ecsSystemIncludes.h"
#include "InputComponents.h"
#include "InputBackendComponent.h"
#include "CameraComponents.h"
#include "UtilityComponents.h"
#include <iostream>
#include <DirectXMath.h>
using namespace DirectX;

namespace ecs
{
	namespace systems
	{
		class Mackes : public ECSSystem<Mackes>
		{
		public:
			Mackes()
			{
				updateType = EntityUpdate;
				typeFilter.addRequirement(components::CameraComponent::typeID);
				typeFilter.addRequirement(components::TransformComponent::typeID);
			}
			virtual ~Mackes() {}
			void updateEntity(FilteredEntity& entity, float delta) override
			{
				//Initialize standard values.
				float speed = 0.20f;
				float sensitivity = 0.01f;
				DirectX::XMVECTOR world_forward = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
				DirectX::XMVECTOR world_right = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
				//Fetch Mouse-, Keyboard-, Camera- and TranformComponent
				ecs::components::MouseComponent *p_mouse;
				ecs::components::KeyboardComponent *p_keyboard;
				
				p_mouse = (components::MouseComponent*)getComponentsOfType(ecs::components::MouseComponent::typeID).next();
				p_keyboard = (components::KeyboardComponent*)getComponentsOfType(ecs::components::KeyboardComponent::typeID).next();
				components::CameraComponent *p_cam = entity.getComponent<components::CameraComponent>();
				components::TransformComponent* p_tc = entity.getComponent<components::TransformComponent>();
				XMMATRIX view;
				XMMATRIX rotation;
				XMVECTOR target;
				XMVECTOR right;
				XMVECTOR forward;
				XMVECTOR up;
				XMVECTOR position;
				//If the Mouse- and KeyboardComponent exists in the ECS we update the cameras position. 
				if (p_mouse && p_keyboard)
				{
					if (p_keyboard->R) //If camera should be reset run this.
					{
						//Reset position, rotation and scale.
						p_tc->position = { 0.0f, 10.0f, 0.0f };
						p_tc->rotation = { 0.0f, 0.0f, 0.0f };
						p_tc->scale = { 1.0f, 1.0f, 1.0f };
						//Reset the cameras target, up, forward and right.
						p_cam->target = { 0.0f, 0.0f, 0.0f, 0.0f };
						p_cam->up = { 0.0f, 1.0f, 0.0f, 0.0f };
						p_cam->forward = { 0.0f, 0.0f, 1.0f, 0.0f };
						p_cam->right = { 1.0f, 0.0f, 0.0f, 0.0f };
						//Update the cameras view matrix.
						position = XMLoadFloat3(&p_tc->position);
						p_cam->viewMatrix = XMMatrixLookAtLH(position, p_cam->target, p_cam->up);
						//Store the matrix in the camera component.
						
					}
					else
					{
						std::cout << "MouseComponent: " << std::endl;
						std::cout << "x: " << p_mouse->diffx << std::endl;
						std::cout << "y: " << p_mouse->diffy << std::endl;
						//Update the cameras rotation vector and matrix with the mouse input.
						p_tc->rotation.y += p_mouse->diffx * sensitivity;
						p_tc->rotation.x += p_mouse->diffy * sensitivity;
						std::cout << "TransformComponent: " << std::endl;
						std::cout << "x: " << p_tc->rotation.y;
						std::cout << "y: " << p_tc->rotation.x;
						p_cam->rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(p_tc->rotation.x, p_tc->rotation.y, 0);
						//Update the cameras target with the new rotation matrix and normalize it.
						p_cam->target = DirectX::XMVector3TransformCoord(world_forward, p_cam->rotationMatrix);
						p_cam->target = DirectX::XMVector3Normalize(p_cam->target);
						//Update the cameras right-, forward- and up vector with the new rotation matrix.
						p_cam->right = DirectX::XMVector3TransformCoord(world_right, p_cam->rotationMatrix);
						p_cam->forward = DirectX::XMVector3TransformCoord(world_forward, p_cam->rotationMatrix);
						p_cam->up = DirectX::XMVector3Cross(p_cam->forward, p_cam->right);

						//Create a local XMVECTOR for the cameras postion for easier math work and update the position of the camera with the
						//keyboard input and the new camera forward- and right vector.
						DirectX::XMVECTOR cam_pos = DirectX::XMVectorSet(p_tc->position.x, p_tc->position.y, p_tc->position.z, 0.0f);
						if (p_keyboard->W)
							cam_pos += speed * p_cam->forward;
						if (p_keyboard->S)
							cam_pos -= speed * p_cam->forward;
						if (p_keyboard->A)
							cam_pos -= speed * p_cam->right;
						if (p_keyboard->D)
							cam_pos += speed * p_cam->right;
						//Store the new position in the TransformComponent position XMFLOAT3.
						DirectX::XMStoreFloat3(&p_tc->position, cam_pos);
						//Update the cameras target and update the view matrix.
						p_cam->target = cam_pos + p_cam->target;
						p_cam->viewMatrix = DirectX::XMMatrixLookAtLH(cam_pos, p_cam->target, p_cam->up);
					}
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