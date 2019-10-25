#include "CombineSSAOPipeline.h"


namespace graphics
{
	CombinePipeline::CombinePipeline()
	{
	}

	CombinePipeline::~CombinePipeline()
	{
	}

	HRESULT CombinePipeline::Initialize(
		ID3D11Device4* pDevice4,
		const void* pDescription)
	{
		HRESULT hr = S_OK;
		COMBINE_PIPELINE_DESC* pDesc = (COMBINE_PIPELINE_DESC*)pDescription;

		m_width = pDesc->Width;
		m_height = pDesc->Height;

		internal::GetBackBuffer(&m_pBackBuffer);

		{
			D3D11_SAMPLER_DESC desc = {};
			desc.Filter = D3D11_FILTER_ANISOTROPIC;
			desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
			desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
			desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
			desc.MaxAnisotropy = 16;
			
			pDevice4->CreateSamplerState(&desc, &m_pSamplerState);
		}
		return S_OK;
	}

	void CombinePipeline::Update(ID3D11DeviceContext4* pContext4, const void* pPipelineData)
	{
		COMBINE_PIPELINE_DATA* pData = (COMBINE_PIPELINE_DATA*)pPipelineData;
	}

	void CombinePipeline::Begin(ID3D11DeviceContext4* pContext4)
	{
		graphics::SetViewport(pContext4, 0, 0, m_width, m_height);
		pContext4->PSSetSamplers(2, 1, &m_pSamplerState);
		pContext4->OMSetRenderTargets(1, &m_pBackBuffer, NULL);
	}

	void CombinePipeline::PreProcess(
		ID3D11DeviceContext4* pContext4,
		ID3D11VertexShader* pVertexShader,
		ID3D11PixelShader* pPixelShader)
	{
		pContext4->VSSetShader(pVertexShader, NULL, 0);
		pContext4->PSSetShader(pPixelShader, NULL, 0);
	}

	void CombinePipeline::End(ID3D11DeviceContext4* pContext4)
	{
	}

	void CombinePipeline::Destroy()
	{
	}
}