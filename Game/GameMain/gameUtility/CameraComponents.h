#pragma once
#include "ecsComponentIncludes.h"
#include <directXMath.h>

enum CAMERA_TYPE
{
	OVERLOOK, DYNAMIC
};

namespace ecs
{
	namespace components
	{
		struct CameraComponent : public ECSComponent<CameraComponent>
		{
			DirectX::XMFLOAT4 target = { 0.0f, 0.0f, 0.0f, 0.0f };
			DirectX::XMFLOAT4 up = { 0.0f, 0.0f, 0.0f, 0.0f };
			DirectX::XMFLOAT4 forward = { 0.0f, 0.0f, 0.0f, 0.0f };
			DirectX::XMFLOAT4 right = { 0.0f, 0.0f, 0.0f, 0.0f };

			DirectX::XMFLOAT4X4 rotationMatrix;
			DirectX::XMFLOAT4X4 projectionMatrix;
			DirectX::XMFLOAT4X4 viewMatrix;

			CAMERA_TYPE type;
		};

		struct PoiComponent : public ECSComponent<PoiComponent>
		{

		};
	}
}