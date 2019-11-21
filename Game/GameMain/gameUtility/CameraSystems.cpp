#include "CameraSystems.h"
#include "CameraEcsFunctions.h"
#include "..//gameAI/AIComponents.h"

using namespace DirectX;
using namespace ecs;
using namespace ecs::components;

/***************************************************************/
/********************* DEV CAMERA SYSTEM **********************/
/*************************************************************/

ecs::systems::FreelookCameraSystem::FreelookCameraSystem()
{
	updateType = EntityUpdate;
	typeFilter.addRequirement(components::CameraComponent::typeID);
	typeFilter.addRequirement(components::TransformComponent::typeID);
	typeFilter.addRequirement(components::FreeLookCameraSystemComponent::typeID);
}

ecs::systems::FreelookCameraSystem::~FreelookCameraSystem()
{

}

void ecs::systems::FreelookCameraSystem::updateEntity(FilteredEntity& entity, float delta)
{
	//Initialize standard values.
	GridProp* p_gp = GridProp::GetInstance();
	float speed = 0.20f;
	float sensitivity = 0.01f;
	DirectX::XMVECTOR world_forward = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	DirectX::XMVECTOR world_right = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	//Fetch Mouse-, Keyboard-, Camera- and TranformComponent
	ecs::components::MouseComponent* p_mouse;
	ecs::components::KeyboardComponent* p_keyboard;

	p_mouse = (components::MouseComponent*)getComponentsOfType(ecs::components::MouseComponent::typeID).next();
	p_keyboard = (components::KeyboardComponent*)getComponentsOfType(ecs::components::KeyboardComponent::typeID).next();
	components::CameraComponent* p_cam = entity.getComponent<components::CameraComponent>();
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
			p_tc->position = CameraDefines::originalPosition;
			p_tc->rotation = CameraDefines::originalRotation;
			p_tc->scale = CameraDefines::originalScale;
			//Reset the cameras target, up, forward and right.
			p_cam->target = CameraDefines::originalTarget;
			p_cam->up = CameraDefines::originalUp;
			p_cam->forward = CameraDefines::originalForward;
			p_cam->right = CameraDefines::originalRight;
			//Update the cameras view matrix.
			position = XMLoadFloat3(&p_tc->position);
			target = XMLoadFloat4(&p_cam->target);
			up = XMLoadFloat4(&p_cam->up);
			view = XMMatrixLookAtLH(position, target, up);
			//Store the matrix in the camera component.
			XMStoreFloat4x4(&p_cam->viewMatrix, view);
		}
		else //If the camers is not supposed to be reset run this.
		{
			rotation = XMLoadFloat4x4(&p_cam->rotationMatrix);
			//Update the cameras rotation vector and matrix with the mouse input.
			p_tc->rotation.y += p_mouse->diffFloat2.x * sensitivity;
			p_tc->rotation.x += p_mouse->diffFloat2.y * sensitivity;
			rotation = DirectX::XMMatrixRotationRollPitchYaw(p_tc->rotation.x, p_tc->rotation.y, 0);
			//Update the cameras target with the new rotation matrix and normalize it.
			target = XMLoadFloat4(&p_cam->target);
			target = DirectX::XMVector3TransformCoord(world_forward, rotation);
			target = DirectX::XMVector3Normalize(target);
			//Update the cameras right-, forward- and up vector with the new rotation matrix.
			right = XMLoadFloat4(&p_cam->right);
			forward = XMLoadFloat4(&p_cam->forward);
			up = XMLoadFloat4(&p_cam->up);
			right = DirectX::XMVector3TransformCoord(world_right, rotation);
			forward = DirectX::XMVector3TransformCoord(world_forward, rotation);
			up = DirectX::XMVector3Cross(forward, right);

			//Create a local XMVECTOR for the cameras postion for easier math work and update the position of the camera with the
			//keyboard input and the new camera forward- and right vector.
			DirectX::XMVECTOR cam_pos = DirectX::XMVectorSet(p_tc->position.x, p_tc->position.y, p_tc->position.z, 0.0f);
			if (p_keyboard->W)
				cam_pos += speed * forward;
			if (p_keyboard->S)
				cam_pos -= speed * forward;
			if (p_keyboard->A)
				cam_pos -= speed * right;
			if (p_keyboard->D)
				cam_pos += speed * right;
			//Store the new position in the TransformComponent position XMFLOAT3.
			DirectX::XMStoreFloat3(&p_tc->position, cam_pos);
			//Update the cameras target and update the view matrix.
			target = cam_pos + target;
			view = DirectX::XMMatrixLookAtLH(cam_pos, target, up);

			//Store the new values in the camera component.
			XMStoreFloat4x4(&p_cam->viewMatrix, view);
			XMStoreFloat4x4(&p_cam->rotationMatrix, rotation);
			XMStoreFloat4(&p_cam->target, target);
			XMStoreFloat4(&p_cam->right, right);
			XMStoreFloat4(&p_cam->forward, forward);
			XMStoreFloat4(&p_cam->up, up);
		}
	}
}

/***************************************************************/
/******************* DYNAMIC CAMERA SYSTEM ********************/
/*************************************************************/

ecs::systems::DynamicCameraSystem::DynamicCameraSystem()
{
	updateType = EntityUpdate;
	typeFilter.addRequirement(components::TransformComponent::typeID);
	typeFilter.addRequirement(components::CameraComponent::typeID);
	typeFilter.addRequirement(components::DynamicCameraSystemComponent::typeID);
}

ecs::systems::DynamicCameraSystem::~DynamicCameraSystem()
{

}

void ecs::systems::DynamicCameraSystem::updateEntity(FilteredEntity& entity, float delta)
{
	CameraComponent* p_cam_component = entity.getComponent<CameraComponent>();
	TransformComponent* p_cam_transform = entity.getComponent<TransformComponent>();
	DynamicCameraSystemComponent* p_dyn_comp = entity.getComponent<DynamicCameraSystemComponent>();

	XMVECTOR cam_target = XMLoadFloat4(&p_cam_component->target);
	XMVECTOR cam_final_target = XMLoadFloat4(&p_dyn_comp->target);
	XMVECTOR cam_target_dir = XMVectorSubtract(cam_final_target, cam_target);
	XMVECTOR cam_target_dir_length = XMVector3Length(cam_target_dir);
	cam_target_dir = XMVector3Normalize(cam_target_dir);

	if (!(XMVectorGetX(cam_target_dir_length) < 0.5f))
	{
		cam_target = cam_target + 0.04f * cam_target_dir;
		XMStoreFloat4(&p_cam_component->target, cam_target);
	}

	XMFLOAT4 targetPos = p_dyn_comp->target;

	XMVECTOR target_pos = XMLoadFloat4(&p_cam_component->target);
	XMVECTOR start_pos = XMLoadFloat3(&p_cam_transform->position) - target_pos;
	XMVECTOR end_pos;// = XMLoadFloat4(&targetPos) - target_pos;
	GetTargetPosition(end_pos, target_pos);
	XMVECTOR new_camera_pos = CameraEcsFunctions::Nlerp(start_pos, end_pos);
	new_camera_pos = target_pos + (XMVector3Length(start_pos) + mPercent * (XMVector3Length(end_pos) - XMVector3Length(start_pos))) * new_camera_pos;
	XMStoreFloat3(&p_cam_transform->position, new_camera_pos);
	CameraEcsFunctions::UpdateViewMatrix(*p_cam_component, *p_cam_transform);
}

void ecs::systems::DynamicCameraSystem::GetTargetPosition(DirectX::XMVECTOR& rTarget, DirectX::XMVECTOR& rLookAt)
{
	const float FOV = (90.f / 2.0f) * PI / 180.f;
	// Zero rTarget
	rTarget = DirectX::XMVectorZero();

	// Get a sum of all point of interests
	TypeFilter poi_filter;
	poi_filter.addRequirement(components::TransformComponent::typeID);
	poi_filter.addRequirement(components::PoiComponent::typeID);

	EntityIterator poi_iterator = ECSUser::getEntitiesByFilter(poi_filter);

	components::TransformComponent* p_poi_transform;
	DirectX::XMVECTOR temp_vec, temp_pos;
	for (FilteredEntity& e : poi_iterator.entities)
	{
		p_poi_transform = e.getComponent<components::TransformComponent>();
		temp_pos = DirectX::XMLoadFloat3(&p_poi_transform->position);
		// Make sure to keep position to origin
		temp_pos -= rLookAt;

		// Add to rTarget
		rTarget += DirectX::XMVector3Normalize(temp_pos);
	}
	// Normalize
	rTarget = DirectX::XMVector3Normalize(rTarget);

	// STEP 2:
	temp_vec = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);	// Global up vector
	DirectX::XMVECTOR right_vec = DirectX::XMVector3Cross(temp_vec, -rTarget);	// Should be normalized
	temp_vec = right_vec * tanf(FOV);	// How much right to go to touch right fustrum wall
	DirectX::XMVECTOR right_wall = DirectX::XMVector3Normalize(-rTarget + temp_vec);
	DirectX::XMVECTOR left_wall = DirectX::XMVector3Normalize(-rTarget - temp_vec);

	// ----------------
	// STEP 3: Find radius distance away from lookat point
	float temp_float;
	float radius = 0.0f;

	for (FilteredEntity& e : poi_iterator.entities)
	{
		p_poi_transform = e.getComponent<components::TransformComponent>();
		temp_pos = DirectX::XMLoadFloat3(&p_poi_transform->position);
		temp_pos -= rLookAt;

		temp_vec = (DirectX::XMVector3Dot(temp_vec, right_wall) / DirectX::XMVector3Dot(right_wall, right_wall)) * right_wall;
		temp_float = DirectX::XMVectorGetX(DirectX::XMVector3Length(temp_pos - temp_vec)) / sinf(FOV);
		radius = fmaxf(radius, temp_float);

		temp_vec = (DirectX::XMVector3Dot(temp_vec, left_wall) / DirectX::XMVector3Dot(left_wall, left_wall)) * left_wall;
		temp_float = DirectX::XMVectorGetX(DirectX::XMVector3Length(temp_pos - temp_vec)) / sinf(FOV);
		radius = fmaxf(radius, temp_float);
	}

	rTarget *= radius;
	//std::cout << radius << std::endl; //Used for debug.
}

void ecs::systems::DynamicCameraSystem::CheckCameraAngle(DirectX::XMVECTOR& cameraPos, DirectX::XMVECTOR& targetPos)
{
	/***********************************************/
				/******* DOES NOT WORK AS INTENDED ATM ********/
				/** MIGHT WANT TO BUILD ON IT IN THE FUTURE **/
				/********************************************/
	XMVECTOR temp_pos = cameraPos;
	XMVECTOR grounded_pos = cameraPos;
	float y = XMVectorGetY(targetPos);
	grounded_pos = XMVectorSetY(grounded_pos, y);
	temp_pos = XMVectorSubtract(temp_pos, targetPos);
	grounded_pos = XMVectorSubtract(grounded_pos, targetPos);
	XMVECTOR length_ground = XMVector3Length(grounded_pos);
	float f_length = XMVectorGetX(length_ground);
	XMVECTOR angle = XMVector3AngleBetweenVectors(grounded_pos, temp_pos);
	float max_angle = PI / 3; //60 degrees
	float true_angle;
	float new_y = XMVectorGetX(angle);
	if (max_angle < new_y)
	{
		true_angle = max_angle;
		f_length = f_length * tan(true_angle);
		y += f_length;
		cameraPos = XMVectorSetY(cameraPos, y);
	}
}


/***************************************************************/
/***************** ARMY SPAWN CAMERA SYSTEM *******************/
/*************************************************************/

ecs::systems::ArmyZoomCameraSystem::ArmyZoomCameraSystem()
{
	updateType = EntityUpdate;
	typeFilter.addRequirement(components::TransformComponent::typeID);
	typeFilter.addRequirement(components::CameraComponent::typeID);
	typeFilter.addRequirement(components::ArmyZoomCameraSystemComponent::typeID);
	mCurrentArmyTarget = 0;
	mZoomIn = true;
	mCameraInPosition = false;
}

ecs::systems::ArmyZoomCameraSystem::~ArmyZoomCameraSystem()
{

}

void ecs::systems::ArmyZoomCameraSystem::updateEntity(FilteredEntity& entity, float delta)
{

	CameraComponent* p_cam_component = entity.getComponent<CameraComponent>();
	TransformComponent* p_cam_transform = entity.getComponent<TransformComponent>();
	ArmyZoomCameraSystemComponent* p_army_zoom = entity.getComponent<ArmyZoomCameraSystemComponent>();

	XMVECTOR cam_pos = XMLoadFloat3(&p_cam_transform->position);
	XMVECTOR cam_target;
	XMVECTOR unit_pos;
	XMVECTOR cam_direction;
	XMVECTOR distance_vector;
	XMVECTOR camera_start_pos = XMLoadFloat3(&p_army_zoom->startPos);

	if (!mCameraInPosition)
	{
		cam_direction = XMVectorSubtract(camera_start_pos, cam_pos);
		cam_direction = XMVector3Normalize(cam_direction);

		cam_pos = cam_pos + (0.1f * cam_direction);
		distance_vector = XMVectorSubtract(cam_pos, camera_start_pos);
		float distance = XMVectorGetX(XMVector3Length(distance_vector));
		if (distance < 1.0f)
		{
			mCameraInPosition = true;
		}
		XMStoreFloat3(&p_cam_transform->position, cam_pos);
		CameraEcsFunctions::UpdateViewMatrix(*p_cam_component, *p_cam_transform);
	}
	else
	{
		std::vector<ArmyComponent*> armies;
		ComponentIterator it = ECSUser::getComponentsOfType<ArmyComponent>();
		BaseComponent* p_base_component;
		it = ECSUser::getComponentsOfType(ArmyComponent::typeID);
		while (p_base_component = it.next())
		{
			armies.push_back(static_cast<ArmyComponent*>(p_base_component));
		}

		if (mCurrentArmyTarget < armies.size())
		{
			if (armies.size() > 0)
			{
				TransformComponent* p_target_unit_transform;
				XMFLOAT3 direction;
				switch (mCurrentArmyTarget)
				{
				case 0:
					p_target_unit_transform = ECSUser::getComponentFromKnownEntity<TransformComponent>(armies[0]->unitIDs[0]);
					unit_pos = XMLoadFloat3(&p_target_unit_transform->position);
					cam_target = XMLoadFloat3(&p_target_unit_transform->position);
					if (mZoomIn)
						cam_direction = XMVectorSubtract(unit_pos, cam_pos);
					else
						cam_direction = XMVectorSubtract(camera_start_pos, cam_pos);
					cam_direction = XMVector3Normalize(cam_direction);
					break;
				case 1:
					p_target_unit_transform = ECSUser::getComponentFromKnownEntity<TransformComponent>(armies[1]->unitIDs[0]);
					unit_pos = XMLoadFloat3(&p_target_unit_transform->position);
					cam_target = XMLoadFloat3(&p_target_unit_transform->position);
					if (mZoomIn)
						cam_direction = XMVectorSubtract(unit_pos, cam_pos);
					else
						cam_direction = XMVectorSubtract(camera_start_pos, cam_pos);
					cam_direction = XMVector3Normalize(cam_direction);
					break;
				case 2:
					p_target_unit_transform = ECSUser::getComponentFromKnownEntity<TransformComponent>(armies[2]->unitIDs[0]);
					unit_pos = XMLoadFloat3(&p_target_unit_transform->position);
					cam_target = XMLoadFloat3(&p_target_unit_transform->position);
					if (mZoomIn)
						cam_direction = XMVectorSubtract(unit_pos, cam_pos);
					else
						cam_direction = XMVectorSubtract(camera_start_pos, cam_pos);
					cam_direction = XMVector3Normalize(cam_direction);
					break;
				case 3:
					p_target_unit_transform = ECSUser::getComponentFromKnownEntity<TransformComponent>(armies[3]->unitIDs[0]);
					unit_pos = XMLoadFloat3(&p_target_unit_transform->position);
					cam_target = XMLoadFloat3(&p_target_unit_transform->position);
					if (mZoomIn)
						cam_direction = XMVectorSubtract(unit_pos, cam_pos);
					else
						cam_direction = XMVectorSubtract(camera_start_pos, cam_pos);
					cam_direction = XMVector3Normalize(cam_direction);
					break;
				default:
					break;
				}


				cam_pos = cam_pos + (0.1f * cam_direction);
				distance_vector = XMVectorSubtract(cam_pos, unit_pos);
				float distance = XMVectorGetX(XMVector3Length(distance_vector));
				if (mZoomIn && distance < 5.0f)
				{
					mZoomIn = false;
					mCameraInPosition = false;
				}
				else if (!mZoomIn)
				{
					mZoomIn = true;
					mCurrentArmyTarget++;
				}
				XMStoreFloat3(&p_cam_transform->position, cam_pos);
				XMStoreFloat4(&p_cam_component->target, cam_target);
				CameraEcsFunctions::UpdateViewMatrix(*p_cam_component, *p_cam_transform);
			}
		}
		else
		{
			DynamicCameraSystemComponent dynamic_camera_comp;
			CameraEcsFunctions::InitDynamicCameraComponent(dynamic_camera_comp);
			ECSUser::createComponent(entity.entity->getID(), dynamic_camera_comp);
			ECSUser::removeComponent(entity.entity->getID(), ArmyZoomCameraSystemComponent::typeID);
		}
	}	
}

/***************************************************************/
/****************** OVERLOOK CAMERA SYSTEM ********************/
/*************************************************************/

ecs::systems::OverlookCameraSystem::OverlookCameraSystem()
{
	typeFilter.addRequirement(components::TransformComponent::typeID);
	typeFilter.addRequirement(components::CameraComponent::typeID);
	typeFilter.addRequirement(components::OverlookCameraSystemComponent::typeID);
}

ecs::systems::OverlookCameraSystem::~OverlookCameraSystem()
{

}

void ecs::systems::OverlookCameraSystem::updateEntity(FilteredEntity& entity, float delta)
{

}