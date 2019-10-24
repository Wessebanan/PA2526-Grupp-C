#include "ShadowMapPipeline.h"

namespace graphics
{
	static void SetOrtographicsMatrix(
		DirectX::XMFLOAT4X4& rOrtographicsMatrix,
		const float width,
		const float height,
		const float nearPlane,
		const float farPlane)
	{
		DirectX::XMStoreFloat4x4(&rOrtographicsMatrix,
			DirectX::XMMatrixOrthographicLH(
				width, height, nearPlane, farPlane));
	}

	ShadowMapPipeline::ShadowMapPipeline()
	{
	}

	ShadowMapPipeline::~ShadowMapPipeline()
	{
	}

	HRESULT ShadowMapPipeline::Initialize(
		ID3D11Device4* pDevice4,
		const void* pDescription)
	{
		SHADOW_MAP_PIPELINE_DESC* pDesc = (SHADOW_MAP_PIPELINE_DESC*)pDescription;

		m_pixelsWidth = pDesc->PixelsWidth;

		{
			D3D11_TEXTURE2D_DESC texture_desc = { 0 };
			texture_desc.Width = m_pixelsWidth;
			texture_desc.Height = m_pixelsWidth;
			texture_desc.ArraySize = 1;
			texture_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
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
		
			D3D11_SHADER_RESOURCE_VIEW_DESC shader_desc = {};
			shader_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			shader_desc.Format = DXGI_FORMAT_R32_FLOAT;
			shader_desc.Texture2D.MipLevels = 1;
			shader_desc.Texture2D.MostDetailedMip = 0;

			pDevice4->CreateShaderResourceView(
				m_pDepthTexture, 
				&shader_desc, 
				&m_pDepthResource);
		}

		{
			DirectX::XMFLOAT4X4 orto_matrix;
			SetOrtographicsMatrix(
				orto_matrix,
				pDesc->Width,
				pDesc->Height,
				pDesc->NearPlane,
				pDesc->FarPlane);

			D3D11_BUFFER_DESC desc = { 0 };
			desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			desc.ByteWidth = sizeof(DirectX::XMFLOAT4X4);
			desc.CPUAccessFlags = 0;
			desc.Usage = D3D11_USAGE_DEFAULT;

			D3D11_SUBRESOURCE_DATA data = { 0 };
			data.pSysMem = &orto_matrix;

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

		{
			D3D11_SAMPLER_DESC desc = {};

			desc.Filter = D3D11_FILTER_COMPARISON_ANISOTROPIC;
			desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
			desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
			desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
			desc.MaxAnisotropy = 16;
			desc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
			desc.BorderColor[0] = 1.0f;
			desc.BorderColor[1] = 1.0f;
			desc.BorderColor[2] = 1.0f;
			desc.BorderColor[3] = 1.0f;

			pDevice4->CreateSamplerState(&desc, &m_pSamplerState);
		}

		return S_OK;
	}

	void ShadowMapPipeline::Update(ID3D11DeviceContext4* pContext4, const void* pPipelineData)
	{
		SHADOW_MAP_PIPELINE_DATA* pData = (SHADOW_MAP_PIPELINE_DATA*)pPipelineData;
		
		graphics::UploadToDynamicBuffer(
			pContext4,
			m_pMatrixBuffers[0],
			D3D11_MAP_WRITE_DISCARD,
			&pData->ViewMatrix,
			sizeof(pData->ViewMatrix),
			0);
	}

	void ShadowMapPipeline::Begin(ID3D11DeviceContext4* pContext4)
	{
		ID3D11ShaderResourceView* pView = NULL;
		pContext4->PSSetShaderResources(0, 1, &pView);

		pContext4->ClearDepthStencilView(m_pDepthBuffer, D3D11_CLEAR_DEPTH, 1.0f, 0);

		graphics::SetViewport(pContext4, 0, 0, m_pixelsWidth, m_pixelsWidth);
		pContext4->OMSetRenderTargets(0, NULL, m_pDepthBuffer);

		pContext4->VSSetConstantBuffers(1, 2, m_pMatrixBuffers);
	}

	void ShadowMapPipeline::PreProcess(
		ID3D11DeviceContext4* pContext4,
		ID3D11VertexShader* pVertexShader,
		ID3D11PixelShader* pPixelShader)
	{
		pContext4->VSSetShader(pVertexShader, NULL, 0);
		pContext4->PSSetShader(NULL, NULL, 0);
	}

	void ShadowMapPipeline::End(ID3D11DeviceContext4* pContext4)
	{
		pContext4->OMSetRenderTargets(0, NULL, NULL);
		pContext4->PSSetSamplers(0, 1, &m_pSamplerState);
		pContext4->VSSetConstantBuffers(3, 2, m_pMatrixBuffers);
		pContext4->PSSetShaderResources(0, 1, &m_pDepthResource);
	}

	void ShadowMapPipeline::Destroy()
	{
		m_pDepthBuffer->Release();
		m_pDepthResource->Release();
		m_pDepthTexture->Release();

		m_pSamplerState->Release();

		m_pMatrixBuffers[0]->Release();
		m_pMatrixBuffers[1]->Release();
	}
}