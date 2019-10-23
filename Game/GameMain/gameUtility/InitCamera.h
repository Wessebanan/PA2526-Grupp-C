#pragma once

#include "CameraEcsFunctions.h"
#include "ecs.h"

void InitCamera(ecs::EntityComponentSystem& rECS)
{
	CameraEcsFunctions::CreateDevCamera(rECS);
	//rECS.createSystem<ecs::systems::UpdateCameraSystem>();
}