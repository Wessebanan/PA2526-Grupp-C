#pragma once
#include "CameraComponents.h"
#include "CameraSystems.h"
#include "CameraEvents.h"
#include "ecs.h"

namespace CameraFunctions
{
	/* Creates a camera entity with a CameraComponent and a Transform component in the ECS system.*/
	void CreateDevCamera(ecs::EntityComponentSystem& rEcs);
	/* Creates camera systems for future use. (For example to move the camera etc.) */
	void CreateCameraSystems(ecs::EntityComponentSystem& rEcs);
}