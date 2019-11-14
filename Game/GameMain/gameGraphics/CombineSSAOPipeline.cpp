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

		mWidth = pDesc->Width;
		mHeight = pDesc->Height;

		internal::GetBackBuffer(&mpBackBuffer);

		return S_OK;
	}

	void CombinePipeline::Update(ID3D11DeviceContext4* pContext4, const void* pPipelineData)
	{
		COMBINE_PIPELINE_DATA* pData = (COMBINE_PIPELINE_DATA*)pPipelineData;
	}

	void CombinePipeline::Begin(ID3D11DeviceContext4* pContext4)
	{
		graphics::SetViewport(pContext4, 0, 0, mWidth, mHeight);
		pContext4->OMSetRenderTargets(1, &mpBackBuffer, NULL);
	}

	void CombinePipeline::Clear(ID3D11DeviceContext4* pContext4)
	{
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