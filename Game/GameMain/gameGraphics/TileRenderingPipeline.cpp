#include "TileRenderingPipeline.h"

namespace graphics
{
	TileRenderingPipeline::TileRenderingPipeline()
	{
		//
	}

	TileRenderingPipeline::~TileRenderingPipeline()
	{
		//
	}

	HRESULT TileRenderingPipeline::Initialize(ID3D11Device4* pDevice4, const void* pDescription)
	{
		HRESULT hr;
		TILE_RENDERING_PIPELINE_DESC* desc = (TILE_RENDERING_PIPELINE_DESC*)pDescription;

		hr = graphics::CreateVertexBuffer(pDevice4, desc->pWorldMesh, desc->size, desc->stride, 0, D3D11_USAGE_DEFAULT, &mpVertexBuffer, &mpVertexBufferView);

		return hr;
	}

	void TileRenderingPipeline::Update(ID3D11DeviceContext4* pContext4, const void* pPipelineData)
	{
		//
	}

	void TileRenderingPipeline::SetState(ID3D11DeviceContext4* pContext4)
	{
		pContext4->VSSetShaderResources(5, 1, &mpVertexBufferView);
	}

	void TileRenderingPipeline::Destroy()
	{
		mpVertexBuffer->Release();
		mpVertexBufferView->Release();
	}
}