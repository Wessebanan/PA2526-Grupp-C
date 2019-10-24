#include "SSAOPipeline.h"

namespace graphics
{

	SSAOPipeline::SSAOPipeline()
	{
	}

	SSAOPipeline::~SSAOPipeline()
	{
	}

	HRESULT SSAOPipeline::Initialize(
		ID3D11Device4* pDevice4,
		const void* pDescription)
	{
		SSAO_PIPELINE_DESC* pDesc = (SSAO_PIPELINE_DESC*)pDescription;

		return S_OK;
	}

	void SSAOPipeline::Update(ID3D11DeviceContext4* pContext4, const void* pPipelineData)
	{
		SSAO_PIPELINE_DATA* pData = (SSAO_PIPELINE_DATA*)pPipelineData;
	}

	void SSAOPipeline::Begin(ID3D11DeviceContext4* pContext4)
	{
	}

	void SSAOPipeline::PreProcess(
		ID3D11DeviceContext4* pContext4,
		ID3D11VertexShader* pVertexShader,
		ID3D11PixelShader* pPixelShader)
	{
		pContext4->VSSetShader(pVertexShader, NULL, 0);
		pContext4->PSSetShader(pPixelShader, NULL, 0);
	}

	void SSAOPipeline::End(ID3D11DeviceContext4* pContext4)
	{
	}

	void SSAOPipeline::Destroy()
	{
	}
}