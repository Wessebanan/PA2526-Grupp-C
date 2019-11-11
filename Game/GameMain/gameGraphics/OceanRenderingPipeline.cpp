#include "OceanRenderingPipeline.h"

namespace graphics
{
	OceanRenderingPipeline::OceanRenderingPipeline()
	{
		//
	}

	OceanRenderingPipeline::~OceanRenderingPipeline()
	{
		//
	}

	HRESULT OceanRenderingPipeline::Initialize(ID3D11Device4* pDevice4, const void* pDescription)
	{
		HRESULT hr;
		OCEAN_RENDERING_PIPELINE_DESC* desc = (OCEAN_RENDERING_PIPELINE_DESC*)pDescription;

		hr = graphics::CreateVertexBuffer(pDevice4, desc->pWorldMesh, desc->size, desc->stride, 0, D3D11_USAGE_DEFAULT, &mpVertexBuffer, &mpVertexBufferView);

		if (FAILED(hr))
		{
			return hr;
		}

		D3D11_BUFFER_DESC buffer_desc = { 0 };
		buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		buffer_desc.ByteWidth = 65536;
		buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
		buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		hr = pDevice4->CreateBuffer(&buffer_desc, 0, &mpDataBuffer);

		return hr;
	}

	void OceanRenderingPipeline::Update(ID3D11DeviceContext4* pContext4, const void* pPipelineData)
	{
		//
		OCEAN_RENDERING_PIPELINE_DATA* pData = (OCEAN_RENDERING_PIPELINE_DATA*)pPipelineData;

		graphics::UploadToDynamicBuffer(
			pContext4,
			mpDataBuffer,
			D3D11_MAP_WRITE_DISCARD,
			pData->pHeightBuffer,
			pData->ByteWidth,
			0);
	}

	void OceanRenderingPipeline::SetState(ID3D11DeviceContext4* pContext4)
	{
		pContext4->VSSetConstantBuffers(6, 1, &mpDataBuffer);
		pContext4->VSSetShaderResources(6, 1, &mpVertexBufferView);
	}

	void OceanRenderingPipeline::Destroy()
	{
		mpVertexBuffer->Release();
		mpVertexBufferView->Release();
	}
}