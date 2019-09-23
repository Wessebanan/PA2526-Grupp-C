#pragma once
#include "ecsComponentIncludes.h"
#include <directXMath.h>

namespace ecs
{
	namespace components
	{
		struct CameraComponent : public ECSComponent<CameraComponent>
		{
			DirectX::XMVECTOR target = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
			DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
			DirectX::XMVECTOR forward = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
			DirectX::XMVECTOR right = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

			DirectX::XMMATRIX rotationMatrix;
			DirectX::XMMATRIX projectionMatrix;
			DirectX::XMMATRIX viewMatrix;
		};
	}
}