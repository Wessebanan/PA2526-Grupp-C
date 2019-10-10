#pragma once
#include "CameraComponents.h"
#include "CameraSystems.h"
#include "CameraEvents.h"
#include "ecs.h"

namespace CameraEcsFunctions
{
	/* Creates a camera entity with a CameraComponent and a Transform component in the ECS system.*/
	void CreateDevCamera(ecs::EntityComponentSystem& rEcs);
}