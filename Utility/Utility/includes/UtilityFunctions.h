#pragma once
#include <DirectXMath.h>
#include <UtilityComponents.h>
#include <UtilityEvents.h>
#include <UtilitySystems.h>

namespace UtilityFunctions {
	/* Generates a world XMMATRIX from the given TransformComponent */
	DirectX::XMMATRIX GetWorldMatrix(ecs::components::TransformComponent tc);
}