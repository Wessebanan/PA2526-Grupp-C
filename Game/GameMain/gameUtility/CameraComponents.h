#pragma once
#include "ecsComponentIncludes.h"
#include <directXMath.h>

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
		};

		struct PoiComponent : public ECSComponent<PoiComponent>
		{
			DirectX::XMFLOAT3 startPos;
			DirectX::XMFLOAT4 target;
		};

		struct FreeLookCameraSystemComponent : public ECSComponent< FreeLookCameraSystemComponent>
		{

		};

		struct OverlookCameraSystemComponent : public ECSComponent<OverlookCameraSystemComponent>
		{
			DirectX::XMFLOAT3 startPos;
			DirectX::XMFLOAT4 target;
		};

		struct StartPosCameraSystemComponent : public ECSComponent<StartPosCameraSystemComponent>
		{
			DirectX::XMFLOAT3 startPos;
			DirectX::XMFLOAT4 target;
		};

		struct DynamicCameraSystemComponent : public ECSComponent<DynamicCameraSystemComponent>
		{
			DirectX::XMFLOAT3 startPos;
			DirectX::XMFLOAT4 target;
		};

		struct ArmyZoomCameraSystemComponent : public ECSComponent<ArmyZoomCameraSystemComponent>
		{
			DirectX::XMFLOAT3 startPos;
			DirectX::XMFLOAT4 target;
		};




	}
}