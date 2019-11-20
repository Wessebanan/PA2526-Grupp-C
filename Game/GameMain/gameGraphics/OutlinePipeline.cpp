#include "OutlinePipeline.h"
#include "../gameUtility/UtilityGraphics.h"

graphics::OutlinePipeline::OutlinePipeline()
{
}

graphics::OutlinePipeline::~OutlinePipeline()
{
}

HRESULT graphics::OutlinePipeline::Initialize(ID3D11Device4* pDevice4, const void* pDescription)
{
	OUTLINE_PIPELINE_DESC* pDesc = (OUTLINE_PIPELINE_DESC*)pDescription;
	this->mpFakeStencilSRV = pDesc->fakeStencilSRV;

	HRESULT hr;

	hr = graphics::CreateVertexShaderFromFile(pDevice4, GetShaderFilepath("VS_Outline.cso").c_str(), &mpFullScreenQuadVS);
	if (FAILED(hr))
	{
		MessageBox(0, "CreatePixelShaderFromFile for PS_Outline.cso failed", 0, 0);
		return E_ABORT;
	}


	hr = graphics::CreatePixelShaderFromFile(pDevice4, GetShaderFilepath("PS_Outline.cso").c_str(), &mpOutlinePS);
	if (FAILED(hr))
	{
		MessageBox(0, "CreatePixelShaderFromFile for PS_Outline.cso failed", 0, 0);
		return E_ABORT;
	}

	return hr;
}

void graphics::OutlinePipeline::Clear(ID3D11DeviceContext4* pContext4)
{
}

void graphics::OutlinePipeline::Update(ID3D11DeviceContext4* pContext4, const void* pPipelineData)
{
}

void graphics::OutlinePipeline::Begin(ID3D11DeviceContext4* pContext4)
{
	pContext4->PSSetShaderResources(6, 1, &this->mpFakeStencilSRV);
	ID3D11RenderTargetView* pBackBuffer;
	internal::GetBackBuffer(&pBackBuffer);
	pContext4->OMSetRenderTargets(1, &pBackBuffer, NULL);
}

void graphics::OutlinePipeline::PreProcess(ID3D11DeviceContext4* pContext4, ID3D11VertexShader* pVertexShader, ID3D11PixelShader* pPixelShader)
{
	pContext4->VSSetShader(this->mpFullScreenQuadVS, NULL, 0);
	pContext4->PSSetShader(this->mpOutlinePS, NULL, 0);
}

void graphics::OutlinePipeline::End(ID3D11DeviceContext4* pContext4)
{
}

void graphics::OutlinePipeline::Destroy()
{
}
