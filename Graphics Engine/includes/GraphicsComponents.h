#pragma once

#include "ecsComponentIncludes.h"

#include "DeviceInterface.h"

namespace ecs
{
	namespace components
	{
		struct WorldComponent : public ECSComponent<WorldComponent>
		{
			// TOOD
			DirectX::XMFLOAT4X4 WorldMatrix;

			// float3 pos
			// float3 rotation
			// float3 scale
		};

		struct MeshComponent : public ECSComponent<MeshComponent>
		{
			graphics::BufferRegion MeshLocation;
		};
	}
}