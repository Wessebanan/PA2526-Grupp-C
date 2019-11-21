#pragma once

#include "CameraEcsFunctions.h"
#include "ecs.h"

void InitCamera(ecs::EntityComponentSystem& rECS)
{
	TransformComponent p_transform;
	CameraComponent p_camera;


	//CameraEcsFunctions::CreateDevCamera(rECS);
	CameraEcsFunctions::CreateOverlookCamera(p_transform, p_camera);
	//CameraEcsFunctions::CreateDynamicCamera(p_transform, p_camera);
	//rECS.createSystem<ecs::systems::UpdateDynamicCameraSystem>();

	//rECS.createSystem<ecs::systems::UpdateCameraSystem>();
#ifdef _DEBUG
#endif //_DEBUG

	//Create the camera entity
	rECS.createEntity(p_transform, p_camera);
}