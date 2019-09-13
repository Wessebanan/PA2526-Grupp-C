#pragma once
#include <ecs.h>
#include "CameraComponents.h"
#include "CameraSystems.h"
#include "CameraEvents.h"

namespace CameraFunctions
{
	void InitDevCamera(ecs::EntityComponentSystem& rEcs);
	void InitCameraSystems(ecs::EntityComponentSystem& rEcs);
}