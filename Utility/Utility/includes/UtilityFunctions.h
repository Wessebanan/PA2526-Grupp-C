#pragma once
#include <DirectXMath.h>
#include <UtilityComponents.h>
#include <UtilityEvents.h>
#include <UtilitySystems.h>

namespace UtilityFunctions
{
	DirectX::XMMATRIX GetWorldMatrix(ecs::components::TransformComponent tc);
}