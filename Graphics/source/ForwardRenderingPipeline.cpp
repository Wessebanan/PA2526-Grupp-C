
#include "../includes/ForwardRenderingPipeline.h"

namespace graphics
{
	void SetViewMatrix(
		DirectX::XMFLOAT4X4& rViewMatrix,
		const float x,
		const float y,
		const float z)
	{
		DirectX::XMStoreFloat4x4(&rViewMatrix,
			DirectX::XMMatrixLookToLH(
				{ x, y, z },
				{ 0.0f, 0.0f,  1.0f },
				{ 0.0f, 1.0f,  0.0f }
		));
	}

	void SetProjectionMatrix(
		DirectX::XMFLOAT4X4& rProjectionMatrix,
		const float fov,
		const float aspectRatio,
		const float nearPlane,
		const float farPlane)
	{
		DirectX::XMStoreFloat4x4(&rProjectionMatrix,
			DirectX::XMMatrixPerspectiveFovLH(
				fov, aspectRatio, nearPlane, farPlane
			));
	}

	ForwardRenderingPipeline::ForwardRenderingPipeline()
	{
	}

	ForwardRenderingPipeline::~ForwardRenderingPipeline()
	{
	}

	HRESULT ForwardRenderingPipeline::Initialize(
		ID3D11Device4* pDevice4,
		const void* pDescription)
	{
		FORWARD_RENDERING_PIPELINE_DESC* pDesc = (FORWARD_RENDERING_PIPELINE_DESC*)pDescription;

		m_clientWidth	= pDesc->ClientWidth;
		m_clientHeight	= pDesc->ClientHeight;

		{
			D3D11_TEXTURE2D_DESC texture_desc = { 0 };
			texture_desc.Width = m_clientWidth;
			texture_desc.Height = m_clientHeight;
			texture_desc.ArraySize = 1;
			texture_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			texture_desc.CPUAccessFlags = 0;
			texture_desc.Format = DXGI_FORMAT_R32_TYPELESS;
			texture_desc.SampleDesc = { 1, 0 };
			texture_desc.Usage = D3D11_USAGE_DEFAULT;

			pDevice4->CreateTexture2D(&texture_desc, NULL, &m_pDepthTexture);

			D3D11_DEPTH_STENCIL_VIEW_DESC depth_desc = {};
			depth_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			depth_desc.Format = DXGI_FORMAT_D32_FLOAT;
			depth_desc.Texture2D.MipSlice = 0;

			pDevice4->CreateDepthStencilView(m_pDepthTexture, &depth_desc, &m_pDepthBuffer);
		}

		{
			D3D11_TEXTURE2D_DESC texture_desc = { 0 };
			texture_desc.Width = m_clientWidth;
			texture_desc.Height = m_clientHeight;
			texture_desc.ArraySize = 1;
			texture_desc.BindFlags = D3D11_BIND_RENDER_TARGET;
			texture_desc.CPUAccessFlags = 0;
			texture_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			texture_desc.SampleDesc = { 1, 0 };
			texture_desc.Usage = D3D11_USAGE_DEFAULT;
			texture_desc.MipLevels = 1;

			pDevice4->CreateTexture2D(&texture_desc, NULL, &m_pTargetTexture);

			D3D11_RENDER_TARGET_VIEW_DESC target_desc = {};
			target_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			target_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			target_desc.Texture2D.MipSlice = 0;

			pDevice4->CreateRenderTargetView(m_pTargetTexture, &target_desc, &m_pRenderTarget);
		}


		{
			DirectX::XMFLOAT4X4 proj_matrix;
			SetProjectionMatrix(
				proj_matrix,
				pDesc->Fov,
				pDesc->ClientWidth / (float)pDesc->ClientHeight,
				pDesc->NearPlane,
				pDesc->FarPlane);

			D3D11_BUFFER_DESC desc = { 0 };
			desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			desc.ByteWidth = sizeof(DirectX::XMFLOAT4X4);
			desc.CPUAccessFlags = 0;
			desc.Usage = D3D11_USAGE_DEFAULT;

			D3D11_SUBRESOURCE_DATA data = { 0 };
			data.pSysMem = &proj_matrix;

			pDevice4->CreateBuffer(&desc, &data, &m_pMatrixBuffers[1]);
		}

		{
			D3D11_BUFFER_DESC desc = { 0 };
			desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			desc.ByteWidth = sizeof(DirectX::XMFLOAT4X4);
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			desc.Usage = D3D11_USAGE_DYNAMIC;

			pDevice4->CreateBuffer(&desc, NULL, &m_pMatrixBuffers[0]);
		}

		return S_OK;
	}

	void ForwardRenderingPipeline::Update(ID3D11DeviceContext4* pContext4, const void* pPipelineData)
	{
		FORWARD_RENDERING_PIPELINE_DATA* pData = (FORWARD_RENDERING_PIPELINE_DATA*)pPipelineData;

		graphics::UploadToDynamicBuffer(
			pContext4, 
			m_pMatrixBuffers[0],
			&pData->ViewMatrix, 
			sizeof(pData->ViewMatrix),
			0);
	}

	void ForwardRenderingPipeline::Begin(ID3D11DeviceContext4* pContext4)
	{
		float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		pContext4->ClearRenderTargetView(m_pRenderTarget, color);
		pContext4->ClearDepthStencilView(m_pDepthBuffer, D3D11_CLEAR_DEPTH, 1.0f, 0);

		graphics::SetViewport(pContext4, 0, 0, m_clientWidth, m_clientHeight);
		pContext4->OMSetRenderTargets(1, &m_pRenderTarget, m_pDepthBuffer);

		pContext4->VSSetConstantBuffers(1, 2, m_pMatrixBuffers);
	}

	void ForwardRenderingPipeline::PreExecute(
		ID3D11DeviceContext4* pContext4,
		ID3D11VertexShader* pVertexShader,
		ID3D11PixelShader* pPixelShader)
	{
		pContext4->VSSetShader(pVertexShader, NULL, 0);
		pContext4->PSSetShader(pPixelShader, NULL, 0);
	}

	void ForwardRenderingPipeline::End(ID3D11DeviceContext4* pContext4, RenderManagerData* pData)
	{
		pContext4->CopyResource(pData->pBackBufferTexture, m_pTargetTexture);
	}

	void ForwardRenderingPipeline::Destroy()
	{
		m_pDepthBuffer->Release();
		m_pDepthTexture->Release();

		m_pRenderTarget->Release();
		m_pTargetTexture->Release();

		m_pMatrixBuffers[0]->Release();
		m_pMatrixBuffers[1]->Release();
	}
}