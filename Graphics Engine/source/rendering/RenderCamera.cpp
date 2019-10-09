#include "../..//includes/rendering/RenderCamera.h"

namespace rendering
{
	RenderCamera::RenderCamera()
	{
	}

	RenderCamera::~RenderCamera()
	{
	}

	void RenderCamera::Initialize(
		const CAMERA_DESC& description,
		graphics::DeviceInterface* pDevice, 
		graphics::RenderContext* pContext,
		bool EnableReadWrite)
	{
		m_width = description.Width;
		m_height = description.Height;

		DirectX::XMFLOAT4X4 projection_matrix;

		if (description.Fov == 0)
		{
			XMStoreFloat4x4(&projection_matrix,
				DirectX::XMMatrixOrthographicLH(
					m_width, 
					m_height, 
					description.NearPlane, 
					description.FarPlane));
		}
		else
		{
			XMStoreFloat4x4(&projection_matrix,
				DirectX::XMMatrixPerspectiveFovLH(
					description.Fov,
					m_width / m_height,
					description.NearPlane,
					description.FarPlane));
		}

		pDevice->CreateStaticBufferRegion(
			sizeof(DirectX::XMFLOAT4X4),
			&projection_matrix,
			&m_matrixRegions[1]);

		pDevice->CreateDynamicBufferRegion(
			sizeof(DirectX::XMFLOAT4X4),
			NULL,
			&m_matrixRegions[0]);

		pDevice->CreateDepthBuffer(
			m_width,
			m_height,
			&m_depthBuffer,
			EnableReadWrite);
	}

	void RenderCamera::SetViewMatrix(const DirectX::XMFLOAT4X4 viewMatrix)
	{
		m_viewMatrix = viewMatrix;
	}

	void RenderCamera::Update(graphics::RenderContext* pContext)
	{
		pContext->CopyDataToRegion(
			&m_matrixRegions,
			sizeof(DirectX::XMFLOAT4X4), 
			m_matrixRegions[0]);
	}

	void RenderCamera::SetAsCamera(
		const UINT startSlot,
		graphics::RenderTarget* pTargetView,
		graphics::RenderContext* pContext)
	{
		pContext->SetViewport(0, 0, m_width, m_height);
		pContext->VSSetConstantBuffers<2>(startSlot, m_matrixRegions);
		pContext->SetRenderTarget(pTargetView, &m_depthBuffer);
	}

	void RenderCamera::SetAsImage(const UINT startSlot, graphics::RenderContext* pContext)
	{
		ID3D11DeviceContext4* pInternal = pContext->GetInternalContext();
		pInternal->PSSetShaderResources(startSlot, 1, &m_depthBuffer.pResource);
	}
}