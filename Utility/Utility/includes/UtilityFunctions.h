#pragma once
#include <DirectXMath.h>
#include <UtilityComponents.h>
#include <UtilityEvents.h>
#include <UtilitySystems.h>

using namespace DirectX;

namespace UtilityFunctions
{
	XMMATRIX GetWorldMatrix(ecs::components::TransformComponent tc);
}