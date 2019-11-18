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
			uint8_t red;
			uint8_t green;
			uint8_t blue;
			uint8_t alpha;
			ColorComponent() : red(0), green(0), blue(0), alpha(0) {}
			ColorComponent(uint8_t r, uint8_t g, uint8_t b) : red(r), green(g), blue(b) {}
		};
	}
}