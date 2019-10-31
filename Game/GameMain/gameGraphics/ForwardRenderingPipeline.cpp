#include "ForwardRenderingPipeline.h"

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

	void SetProjectionMatrixInverse(
		DirectX::XMFLOAT4X4& rProjectionMatrix,
		const float fov,
		const float aspectRatio,
		const float nearPlane,
		const float farPlane)
	{
		DirectX::XMMATRIX projection = DirectX::XMMatrixPerspectiveFovLH(
			fov, aspectRatio, nearPlane, farPlane);

		DirectX::XMVECTOR determinant = DirectX::XMMatrixDeterminant(projection);

		DirectX::XMStoreFloat4x4(&rProjectionMatrix,
			DirectX::XMMatrixInverse(&determinant, projection));
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

		mClientWidth = pDesc->ClientWidth;
		mClientHeight = pDesc->ClientHeight;

		{
			D3D11_TEXTURE2D_DESC texture_desc = { 0 };
			texture_desc.Width = mClientWidth;
			texture_desc.Height = mClientHeight;
			texture_desc.ArraySize = 1;
			texture_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
			texture_desc.CPUAccessFlags = 0;
			texture_desc.Format = DXGI_FORMAT_R32_TYPELESS;
			texture_desc.SampleDesc = { 1, 0 };
			texture_desc.Usage = D3D11_USAGE_DEFAULT;

			pDevice4->CreateTexture2D(&texture_desc, NULL, &mpDepthTexture);

			D3D11_DEPTH_STENCIL_VIEW_DESC depth_desc = {};
			depth_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			depth_desc.Format = DXGI_FORMAT_D32_FLOAT;
			depth_desc.Texture2D.MipSlice = 0;

			pDevice4->CreateDepthStencilView(mpDepthTexture, &depth_desc, &mpDepthBuffer);
		
			D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
			srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srv_desc.Format = DXGI_FORMAT_R32_FLOAT;
			srv_desc.Texture2D.MipLevels = 1;
			srv_desc.Texture2D.MostDetailedMip = 0;

			pDevice4->CreateShaderResourceView(mpDepthTexture, &srv_desc, &mpDepthResource);

		}

		{
			D3D11_BUFFER_DESC desc = { 0 };
			desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			desc.ByteWidth = sizeof(DirectX::XMFLOAT4X4);
			desc.CPUAccessFlags = 0;
			desc.Usage = D3D11_USAGE_DEFAULT;

			{
				DirectX::XMFLOAT4X4 proj_matrix;
				SetProjectionMatrix(
					proj_matrix,
					pDesc->Fov,
					pDesc->ClientWidth / (float)pDesc->ClientHeight,
					pDesc->NearPlane,
					pDesc->FarPlane);

				D3D11_SUBRESOURCE_DATA data = { 0 };
				data.pSysMem = &proj_matrix;

				pDevice4->CreateBuffer(&desc, &data, &mpMatrixBuffers[1]);
			}

			{
				DirectX::XMFLOAT4X4 proj_matrix;
				SetProjectionMatrixInverse(
					proj_matrix,
					pDesc->Fov,
					pDesc->ClientWidth / (float)pDesc->ClientHeight,
					pDesc->NearPlane,
					pDesc->FarPlane);

				D3D11_SUBRESOURCE_DATA data = { 0 };
				data.pSysMem = &proj_matrix;

				pDevice4->CreateBuffer(&desc, &data, &mpMatrixBuffers[2]);
			}
		}

		{
			D3D11_BUFFER_DESC desc = { 0 };
			desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			desc.ByteWidth = sizeof(DirectX::XMFLOAT4X4);
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			desc.Usage = D3D11_USAGE_DYNAMIC;

			pDevice4->CreateBuffer(&desc, NULL, &mpMatrixBuffers[0]);
		}

		// Color Render Target
		{
			ID3D11Texture2D* pTexture = NULL;
			{
				D3D11_TEXTURE2D_DESC desc = { 0 };
				desc.Width = pDesc->ClientWidth;
				desc.Height = pDesc->ClientHeight;
				desc.ArraySize = 1;
				desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
				desc.CPUAccessFlags = 0;
				desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				desc.MipLevels = 1;
				desc.MiscFlags = 0;
				desc.SampleDesc = { 1, 0 };
				desc.Usage = D3D11_USAGE_DEFAULT;

				pDevice4->CreateTexture2D(&desc, NULL, &pTexture);
			}
			{
				D3D11_RENDER_TARGET_VIEW_DESC desc = {};
				desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
				desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				desc.Texture2D.MipSlice = 0;

				pDevice4->CreateRenderTargetView(pTexture, &desc, &mpRenderTargets[0]);
			}
			{
				D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
				desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				desc.Texture2D.MipLevels = 1;
				desc.Texture2D.MostDetailedMip = 0;

				pDevice4->CreateShaderResourceView(
					pTexture,
					&desc,
					&mpShaderResources[0]);
			}
			pTexture->Release();
		}

		// Normal Render Target
		{
			ID3D11Texture2D* pTexture = NULL;
			{
				D3D11_TEXTURE2D_DESC desc = { 0 };
				desc.Width = pDesc->ClientWidth;
				desc.Height = pDesc->ClientHeight;
				desc.ArraySize = 1;
				desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
				desc.CPUAccessFlags = 0;
				desc.Format = DXGI_FORMAT_R8G8B8A8_SNORM;
				desc.MipLevels = 1;
				desc.MiscFlags = 0;
				desc.SampleDesc = { 1, 0 };
				desc.Usage = D3D11_USAGE_DEFAULT;

				pDevice4->CreateTexture2D(&desc, NULL, &pTexture);
			}
			{
				D3D11_RENDER_TARGET_VIEW_DESC desc = {};
				desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
				desc.Format = DXGI_FORMAT_R8G8B8A8_SNORM;
				desc.Texture2D.MipSlice = 0;

				pDevice4->CreateRenderTargetView(pTexture, &desc, &mpRenderTargets[1]);
			}
			{
				D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
				desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				desc.Format = DXGI_FORMAT_R8G8B8A8_SNORM;
				desc.Texture2D.MipLevels = 1;
				desc.Texture2D.MostDetailedMip = 0;

				pDevice4->CreateShaderResourceView(
					pTexture,
					&desc,
					&mpShaderResources[1]);
			}
			pTexture->Release();
		}

		{
			D3D11_SAMPLER_DESC desc = {};

			desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
			desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

			pDevice4->CreateSamplerState(&desc, &mpSamplerState);
		}

		return S_OK;
	}

	void ForwardRenderingPipeline::Update(ID3D11DeviceContext4* pContext4, const void* pPipelineData)
	{
		FORWARD_RENDERING_PIPELINE_DATA* pData = (FORWARD_RENDERING_PIPELINE_DATA*)pPipelineData;

		graphics::UploadToDynamicBuffer(
			pContext4,
			mpMatrixBuffers[0],
			D3D11_MAP_WRITE_DISCARD,
			&pData->ViewMatrix,
			sizeof(pData->ViewMatrix),
			0);

		float color[4] = { pData->Red, pData->Green, pData->Blue, 1.0f };
		float color_clear[4] = { 0.0f, 0.0f, 0.0f, 1.0f };		
		pContext4->ClearRenderTargetView(mpRenderTargets[0], color);
		pContext4->ClearRenderTargetView(mpRenderTargets[1], color_clear);

		pContext4->ClearDepthStencilView(mpDepthBuffer, D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	void ForwardRenderingPipeline::Begin(ID3D11DeviceContext4* pContext4)
	{
		ID3D11ShaderResourceView* pNULLView[2] = { NULL };
		pContext4->PSSetShaderResources(1, 2, pNULLView);
		pContext4->PSSetShaderResources(5, 1, pNULLView);

		graphics::SetViewport(pContext4, 0, 0, mClientWidth, mClientHeight);

		pContext4->OMSetRenderTargets(2, mpRenderTargets, mpDepthBuffer);

		pContext4->VSSetConstantBuffers(1, 2, mpMatrixBuffers);
	}

	void ForwardRenderingPipeline::PreProcess(
		ID3D11DeviceContext4* pContext4,
		ID3D11VertexShader* pVertexShader,
		ID3D11PixelShader* pPixelShader)
	{
		pContext4->VSSetShader(pVertexShader, NULL, 0);
		pContext4->PSSetShader(pPixelShader, NULL, 0);
	}

	void ForwardRenderingPipeline::End(ID3D11DeviceContext4* pContext4)
	{
		ID3D11RenderTargetView* pNULLView[2] = { NULL };
		pContext4->OMSetRenderTargets(2, pNULLView, NULL);

		pContext4->PSSetConstantBuffers(0, 1, &mpMatrixBuffers[2]);

		pContext4->PSSetShaderResources(1, 2, mpShaderResources);
		pContext4->PSSetShaderResources(5, 1, &mpDepthResource);
		pContext4->PSSetSamplers(1, 1, &mpSamplerState);
	}

	void ForwardRenderingPipeline::Destroy()
	{
		mpDepthBuffer->Release();
		mpDepthTexture->Release();

		mpRenderTargets[0]->Release();
		mpRenderTargets[1]->Release();

		mpShaderResources[0]->Release();
		mpShaderResources[1]->Release();

		mpMatrixBuffers[0]->Release();
		mpMatrixBuffers[1]->Release();
		mpMatrixBuffers[2]->Release();

		mpSamplerState->Release();
	}
}