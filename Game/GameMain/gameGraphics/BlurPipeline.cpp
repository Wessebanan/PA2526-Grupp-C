#include "BlurPipeline.h"


namespace graphics
{
	BlurPipeline::BlurPipeline()
	{
	}

	BlurPipeline::~BlurPipeline()
	{
	}

	HRESULT BlurPipeline::Initialize(
		ID3D11Device4* pDevice4,
		const void* pDescription)
	{
		HRESULT hr = S_OK;
		BLUR_PIPELINE_DESC* pDesc = (BLUR_PIPELINE_DESC*)pDescription;

		m_width = pDesc->Width;
		m_height = pDesc->Height;

		// SSAO Render Target
		{
			ID3D11Texture2D* pTexture = NULL;
			{
				D3D11_TEXTURE2D_DESC desc = { 0 };
				desc.Width = m_width;
				desc.Height = m_height;
				desc.ArraySize = 1;
				desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
				desc.CPUAccessFlags = 0;
				desc.Format = DXGI_FORMAT_R8_UNORM;
				desc.MipLevels = 0;
				desc.MiscFlags = 0;
				desc.SampleDesc = { 1, 0 };
				desc.Usage = D3D11_USAGE_DEFAULT;

				pDevice4->CreateTexture2D(&desc, NULL, &pTexture);
			}

			{
				D3D11_RENDER_TARGET_VIEW_DESC desc = {};
				desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
				desc.Format = DXGI_FORMAT_R8_UNORM;
				desc.Texture2D.MipSlice = 0;

				pDevice4->CreateRenderTargetView(pTexture, &desc, &m_pBlurTarget);
			}

			{
				D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
				desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				desc.Format = DXGI_FORMAT_R8_UNORM;
				desc.Texture2D.MipLevels = 1;
				desc.Texture2D.MostDetailedMip = 0;

				pDevice4->CreateShaderResourceView(
					pTexture,
					&desc,
					&m_pBlurResource);
			}

			pTexture->Release();
		}

		{
			D3D11_SAMPLER_DESC desc = {};
			desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
			desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
			desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
			desc.MaxAnisotropy = 16;

			pDevice4->CreateSamplerState(&desc, &m_pSamplerState);
		}
		return S_OK;
	}

	void BlurPipeline::Update(ID3D11DeviceContext4* pContext4, const void* pPipelineData)
	{
		//BLUR_PIPELINE_DATA* pData = (BLUR_PIPELINE_DATA*)pPipelineData;
	}

	void BlurPipeline::Begin(ID3D11DeviceContext4* pContext4)
	{
		graphics::SetViewport(pContext4, 0, 0, m_width, m_height);
		pContext4->PSSetSamplers(2, 1, &m_pSamplerState);
		pContext4->OMSetRenderTargets(1, &m_pBlurTarget, NULL);
	}

	void BlurPipeline::PreProcess(
		ID3D11DeviceContext4* pContext4,
		ID3D11VertexShader* pVertexShader,
		ID3D11PixelShader* pPixelShader)
	{
		pContext4->VSSetShader(pVertexShader, NULL, 0);
		pContext4->PSSetShader(pPixelShader, NULL, 0);
	}

	void BlurPipeline::End(ID3D11DeviceContext4* pContext4)
	{
		ID3D11RenderTargetView* pNull = { NULL };
		pContext4->OMSetRenderTargets(1, &pNull, NULL);

		pContext4->PSSetShaderResources(2, 1, &m_pBlurResource);
	}

	void BlurPipeline::Destroy()
	{
		m_pSamplerState->Release();
	}
}