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
			DirectX::XMFLOAT3 scale = { 1.0f , 1.0f , 1.0f };
		};
		struct ColorComponent : public ECSComponent<ColorComponent>
		{
			uint8_t red = 0;
			uint8_t green = 0;
			uint8_t blue = 0;
		};
	}
}