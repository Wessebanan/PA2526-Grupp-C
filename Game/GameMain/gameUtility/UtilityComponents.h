#pragma once
#include "ecsComponentIncludes.h"
#include <DirectXMath.h>

namespace ecs
{
	namespace components
	{
		struct TransformComponent : public ECSComponent<TransformComponent>
		{
			DirectX::XMFLOAT3 position = { 0.0f , 0.0f , 0.0f };
			DirectX::XMFLOAT3 rotation = { 0.0f , 0.0f , 0.0f };
			DirectX::XMFLOAT3 scale = { 0.0f , 0.0f , 0.0f };
		};
		struct ColorComponent : public ECSComponent<ColorComponent>
		{
			DirectX::XMFLOAT4 color = { 0.0f , 0.0f , 0.0f , 0.0f};
		};
	}
}