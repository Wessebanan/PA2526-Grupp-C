#pragma once

#include "CameraEcsFunctions.h"
#include "ecs.h"

void InitCamera(ecs::EntityComponentSystem& rECS)
{
	//Create all of the camera systems
	rECS.createSystem<ecs::systems::FreelookCameraSystem>(1);
	rECS.createSystem<ecs::systems::DynamicCameraSystem>(1);
	rECS.createSystem<ecs::systems::ArmyZoomCameraSystem>(1);
	rECS.createSystem<ecs::systems::OverlookCameraSystem>(1);
	//Create the camera entity
	ecs::components::TransformComponent p_transform;
	ecs::components::CameraComponent p_camera;
	ecs::Entity* p_cam = rECS.createEntity(p_transform, p_camera);
	//Give the camera entity an OverlookCameraSystemComponent since that is the first state
	CameraEcsFunctions::InitializeCamera(rECS, *p_cam);
}