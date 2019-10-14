#pragma once

#include "RenderHeader.h"
#include <DirectXMath.h>

namespace rendering
{
	struct CAMERA_DESC
	{
		float 
			Width, 
			Height, 
			NearPlane, 
			FarPlane, 
			Fov;
	};

	class RenderCamera
	{
	public:
		RenderCamera();
		~RenderCamera();

		void Initialize(
			const CAMERA_DESC& description,
			graphics::DeviceInterface* pDevice, 
			graphics::RenderContext* pContext,
			bool EnableReadWrite);

		void SetViewMatrix(const DirectX::XMFLOAT4X4 viewMatrix);

		void Update(graphics::RenderContext* pContext);

		void SetAsCamera(
			const UINT startSlot,
			graphics::RenderTarget* pTargetView,
			graphics::RenderContext* pContext);

		void SetAsImage(
			const UINT startSlot,
			graphics::RenderContext* pContext);

	private:
		graphics::BufferRegion m_matrixRegions[2];

		graphics::DepthBuffer m_depthBuffer;
		float m_width, m_height;

		DirectX::XMFLOAT4X4 m_viewMatrix;
	};
}