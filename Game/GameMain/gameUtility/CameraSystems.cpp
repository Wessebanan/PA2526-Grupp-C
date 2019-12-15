#include "CameraSystems.h"

using namespace DirectX;

/***************************************************************/
/********************* DEV CAMERA SYSTEM **********************/
/*************************************************************/

ecs::systems::UpdateCameraSystem::UpdateCameraSystem()
{
	updateType = EntityUpdate;
	typeFilter.addRequirement(components::CameraComponent::typeID);
	typeFilter.addRequirement(components::TransformComponent::typeID);
}

ecs::systems::UpdateCameraSystem::~UpdateCameraSystem()
{

}

void ecs::systems::UpdateCameraSystem::updateEntity(FilteredEntity& entity, float delta)
{
	const XMFLOAT3 center = getComponentFromKnownEntity<components::TransformComponent>(GridProp::GetInstance()->mGrid[9][9].Id)->position;

	const float ROTATION_SPEED = 1.0f;

	components::CameraComponent* p_camera_comp = entity.getComponent<components::CameraComponent>();
	components::TransformComponent* p_transform_comp = entity.getComponent<components::TransformComponent>();

	p_transform_comp->position.y = 20.0f;

	XMMATRIX move_away = XMMatrixTranslationFromVector(XMLoadFloat3(&center));
	XMMATRIX rotate = XMMatrixRotationAxis({ 0.f, 0.1, 0.f, 0.f }, ROTATION_SPEED * delta);
	XMMATRIX move_back = XMMatrixTranslationFromVector(XMLoadFloat3(&center) * -1.f);

	XMStoreFloat3(
		&p_transform_comp->position,
		XMVector3TransformCoord(
			XMLoadFloat3(&p_transform_comp->position),
			move_back * rotate * move_away));
			
	p_camera_comp->target = { center.x, center.y - 4.f, center.z, 0.f };
	p_camera_comp->up = { 0.f, 1.f, 0.f, 0.f };

	XMStoreFloat4x4(
		&p_camera_comp->viewMatrix,
			XMMatrixLookAtLH(
			XMLoadFloat3(&p_transform_comp->position),
			XMLoadFloat4(&p_camera_comp->target),
			{ 0.f, 1.f, 0.f, 0.f }));

	////Initialize standard values.
	//GridProp* p_gp = GridProp::GetInstance();
	//float speed = 0.20f;
	//float sensitivity = 0.01f;
	//DirectX::XMVECTOR world_forward = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	//DirectX::XMVECTOR world_right = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	////Fetch Mouse-, Keyboard-, Camera- and TranformComponent
	//ecs::components::MouseComponent* p_mouse;
	//ecs::components::KeyboardComponent* p_keyboard;

	//p_mouse = (components::MouseComponent*)getComponentsOfType(ecs::components::MouseComponent::typeID).next();
	//p_keyboard = (components::KeyboardComponent*)getComponentsOfType(ecs::components::KeyboardComponent::typeID).next();
	//components::CameraComponent* p_cam = entity.getComponent<components::CameraComponent>();
	//components::TransformComponent* p_tc = entity.getComponent<components::TransformComponent>();

	//XMMATRIX view;
	//XMMATRIX rotation;
	//XMVECTOR target;
	//XMVECTOR right;
	//XMVECTOR forward;
	//XMVECTOR up;
	//XMVECTOR position;
	////If the Mouse- and KeyboardComponent exists in the ECS we update the cameras position. 
	//if (p_mouse && p_keyboard)
	//{
	//	//// Saved if we need to be able to reset the camera on R again

	//	//if (p_keyboard->R) //If camera should be reset run this.
	//	//{
	//	//	//Reset position, rotation and scale.
	//	//	p_tc->position = CameraDefines::originalPosition;
	//	//	p_tc->rotation = CameraDefines::originalRotation;
	//	//	p_tc->scale = CameraDefines::originalScale;
	//	//	//Reset the cameras target, up, forward and right.
	//	//	p_cam->target = CameraDefines::originalTarget;
	//	//	p_cam->up = CameraDefines::originalUp;
	//	//	p_cam->forward = CameraDefines::originalForward;
	//	//	p_cam->right = CameraDefines::originalRight;
	//	//	//Update the cameras view matrix.
	//	//	position = XMLoadFloat3(&p_tc->position);
	//	//	target = XMLoadFloat4(&p_cam->target);
	//	//	up = XMLoadFloat4(&p_cam->up);
	//	//	view = XMMatrixLookAtLH(position, target, up);
	//	//	//Store the matrix in the camera component.
	//	//	XMStoreFloat4x4(&p_cam->viewMatrix, view);
	//	//}
	//	//else //If the camers is not supposed to be reset run this.
	//	{
	//		rotation = XMLoadFloat4x4(&p_cam->rotationMatrix);
	//		//Update the cameras rotation vector and matrix with the mouse input.
	//		p_tc->rotation.y += p_mouse->diffFloat2.x * sensitivity;
	//		p_tc->rotation.x += p_mouse->diffFloat2.y * sensitivity;
	//		rotation = DirectX::XMMatrixRotationRollPitchYaw(p_tc->rotation.x, p_tc->rotation.y, 0);
	//		//Update the cameras target with the new rotation matrix and normalize it.
	//		target = XMLoadFloat4(&p_cam->target);
	//		target = DirectX::XMVector3TransformCoord(world_forward, rotation);
	//		target = DirectX::XMVector3Normalize(target);
	//		//Update the cameras right-, forward- and up vector with the new rotation matrix.
	//		right = XMLoadFloat4(&p_cam->right);
	//		forward = XMLoadFloat4(&p_cam->forward);
	//		up = XMLoadFloat4(&p_cam->up);
	//		right = DirectX::XMVector3TransformCoord(world_right, rotation);
	//		forward = DirectX::XMVector3TransformCoord(world_forward, rotation);
	//		up = DirectX::XMVector3Cross(forward, right);

	//		//Create a local XMVECTOR for the cameras postion for easier math work and update the position of the camera with the
	//		//keyboard input and the new camera forward- and right vector.
	//		DirectX::XMVECTOR cam_pos = DirectX::XMVectorSet(p_tc->position.x, p_tc->position.y, p_tc->position.z, 0.0f);
	//		if (p_keyboard->W)
	//			cam_pos += speed * forward;
	//		if (p_keyboard->S)
	//			cam_pos -= speed * forward;
	//		if (p_keyboard->A)
	//			cam_pos -= speed * right;
	//		if (p_keyboard->D)
	//			cam_pos += speed * right;
	//		//Store the new position in the TransformComponent position XMFLOAT3.
	//		DirectX::XMStoreFloat3(&p_tc->position, cam_pos);
	//		//Update the cameras target and update the view matrix.
	//		target = cam_pos + target;
	//		view = DirectX::XMMatrixLookAtLH(cam_pos, target, up);

	//		//Store the new values in the camera component.
	//		XMStoreFloat4x4(&p_cam->viewMatrix, view);
	//		XMStoreFloat4x4(&p_cam->rotationMatrix, rotation);
	//		XMStoreFloat4(&p_cam->target, target);
	//		XMStoreFloat4(&p_cam->right, right);
	//		XMStoreFloat4(&p_cam->forward, forward);
	//		XMStoreFloat4(&p_cam->up, up);
	//	}
	//}
}

/***************************************************************/
/******************* DYNAMIC CAMERA SYSTEM ********************/
/*************************************************************/

ecs::systems::UpdateDynamicCameraSystem::UpdateDynamicCameraSystem()
{
	updateType = EntityUpdate;
	typeFilter.addRequirement(components::TransformComponent::typeID);
	typeFilter.addRequirement(components::CameraComponent::typeID);
}

ecs::systems::UpdateDynamicCameraSystem::~UpdateDynamicCameraSystem()
{

}

void ecs::systems::UpdateDynamicCameraSystem::updateEntity(FilteredEntity& entity, float delta)
{
	ecs::components::CameraComponent* p_cam_component = entity.getComponent<ecs::components::CameraComponent>();
	ecs::components::TransformComponent* p_cam_transform = entity.getComponent<ecs::components::TransformComponent>();
	XMFLOAT4 targetPos = p_cam_component->target;
	targetPos.x += 2.0f;
	targetPos.y += 2.0f;

	XMVECTOR target_pos = XMLoadFloat4(&p_cam_component->target);
	XMVECTOR start_pos = XMLoadFloat3(&p_cam_transform->position) - target_pos;
	XMVECTOR end_pos;// = XMLoadFloat4(&targetPos) - target_pos;
	GetTargetPosition(end_pos, target_pos);
	XMVECTOR new_camera_pos = this->Nlerp(start_pos, end_pos);
	new_camera_pos = target_pos + (XMVector3Length(start_pos) + mPercent * (XMVector3Length(end_pos) - XMVector3Length(start_pos))) * new_camera_pos;
	XMStoreFloat3(&p_cam_transform->position, new_camera_pos);
	this->UpdateViewMatrix(*p_cam_component, *p_cam_transform);
}

void ecs::systems::UpdateDynamicCameraSystem::GetTargetPosition(DirectX::XMVECTOR& rTarget, DirectX::XMVECTOR& rLookAt)
{
	const float FOV = CameraDefines::fovAngle / 2.0f; //(90.f / 2.0f) * PI / 180.f;
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

void ecs::systems::UpdateDynamicCameraSystem::CheckCameraAngle(DirectX::XMVECTOR& cameraPos, DirectX::XMVECTOR& targetPos)
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

XMVECTOR ecs::systems::UpdateDynamicCameraSystem::Lerp(const XMVECTOR& v1, const XMVECTOR& v2)
{
	XMVECTOR vec_lerp = v1 + mPercent * (v2 - v1);
	return vec_lerp;
}

XMFLOAT3 ecs::systems::UpdateDynamicCameraSystem::Slerp(const XMFLOAT3& v1, const XMFLOAT3& v2)
{
	//Initialize variables and store the vectors XMVECTOR:s so that we can work with them.
	XMFLOAT3 return_position;
	float omega;
	XMVECTOR vec_1 = XMLoadFloat3(&v1);
	XMVECTOR vec_2 = XMLoadFloat3(&v2);
	XMVECTOR vec_1_norm = XMVector3Normalize(vec_1);
	XMVECTOR vec_2_norm = XMVector3Normalize(vec_2);
	XMVECTOR new_position;
	XMVECTOR dot;
	//Calculate dot between the two positions on the dome.
	dot = XMVector3Dot(vec_1_norm, vec_2_norm);
	omega = XMVectorGetX(dot);
	omega = asinf(omega);
	//Calculate the new position along the dome using the Slerp algorithm.
	new_position = (sinf((1.0f - mT) * omega) * vec_1_norm + sinf(mT * omega) * vec_2_norm) / sinf(omega);

	//(1.0f - mT)* vec_1 + mT * vec_2;//

	XMStoreFloat3(&return_position, new_position);
	//return v1;
	return return_position;
}

XMVECTOR ecs::systems::UpdateDynamicCameraSystem::Nlerp(const XMVECTOR& v1, const XMVECTOR& v2)
{
	XMVECTOR vec_norm = Lerp(v1, v2);
	vec_norm = XMVector3Normalize(vec_norm);
	return vec_norm;
}

void ecs::systems::UpdateDynamicCameraSystem::UpdateViewMatrix(ecs::components::CameraComponent& cam, ecs::components::TransformComponent& camTransform)
{
	XMMATRIX view = XMMatrixIdentity();
	XMVECTOR cam_pos = XMLoadFloat3(&camTransform.position);
	XMVECTOR cam_up = XMLoadFloat4(&cam.up);
	XMVECTOR cam_target = XMLoadFloat4(&cam.target);
	view = XMMatrixLookAtLH(cam_pos, cam_target, cam_up);
	XMStoreFloat4x4(&cam.viewMatrix, view);
}

