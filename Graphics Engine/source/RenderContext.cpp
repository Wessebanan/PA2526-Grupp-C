#include "../includes/DeviceInterface.h"
#include <DirectXMath.h>

namespace graphics
{
	using namespace DirectX;

	RenderContext::RenderContext()
	{
		m_viewport = { 0 };
		m_viewport.MaxDepth = 1.0f;
	
		m_pCurrentPipeline = NULL;
		ZeroMemory(m_clearColor, sizeof(m_clearColor));
	}

	RenderContext::~RenderContext()
	{
	}

	void RenderContext::Initialize(ID3D11Device4* pDevice4, InternalStorage* pStorage)
	{
		m_pStorage = pStorage;

		// Set Vertex Buffers

		ID3D11Buffer* pVertexBuffers[4] = {
			m_pStorage->GetBufferHeapGPU(BUFFER_VERTEX_POSITION),
			m_pStorage->GetBufferHeapGPU(BUFFER_VERTEX_NORMAL),
			m_pStorage->GetBufferHeapGPU(BUFFER_VERTEX_UV),
			m_pStorage->GetBufferHeapGPU(BUFFER_VERTEX_ARRAY_INDEX),
		};

		UINT strides[4] = {
			sizeof(float) * 3,
			sizeof(float) * 3,
			sizeof(float) * 2,
			sizeof(UINT) * 1,
		};

		UINT offsets[4] = {
			0,
			0,
			0,
			0,
		};

		m_pContext4->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		m_pContext4->IASetVertexBuffers(
			0,
			ARRAYSIZE(pVertexBuffers),
			pVertexBuffers,
			strides,
			offsets);

		m_pContext4->IASetIndexBuffer(
			m_pStorage->GetBufferHeapGPU(BUFFER_VERTEX_INDEX),
			DXGI_FORMAT_R32_UINT,
			0);
	}

	void RenderContext::Release()
	{
		m_pContext4->Release();
	}

	void RenderContext::ClearDepth(const DepthBuffer& depthBuffer)
	{
		m_pContext4->ClearDepthStencilView(
			depthBuffer.pView,
			D3D11_CLEAR_DEPTH,
			1.0f,
			0);
	}

	void RenderContext::ClearRenderTarget(
		const RenderTarget& renderTarget,
		const float red,
		const float green,
		const float blue)
	{
		m_clearColor[0] = red;
		m_clearColor[1] = green;
		m_clearColor[2] = blue;

		m_pContext4->ClearRenderTargetView(
			renderTarget.pView,
			m_clearColor);
	}

	void RenderContext::CopyDataToRegion(
		const void* pData,
		const UINT byteWidth,
		const BufferRegion& region)
	{
		BufferHeap* pHeap = m_pStorage->GetBufferHeapCPU(region.Type);
		pHeap->CopyDataToRegion(pData, region);
	}

	void RenderContext::UploadToGPU(const BUFFER_TYPE type)
	{
		BufferHeap* pHeap = m_pStorage->GetBufferHeapCPU(type);
		pHeap->UploadToGPU(m_pContext4, type);
	}

	void RenderContext::UploadStaticDataToGPU()
	{
		UploadToGPU(BUFFER_CONSTANT_STATIC);
	}

	void RenderContext::UploadDynamicDataToGPU()
	{
		UploadToGPU(BUFFER_CONSTANT_DYNAMIC);
	}

	void RenderContext::UploadMeshesToGPU()
	{
		UploadToGPU(BUFFER_VERTEX_POSITION);
		UploadToGPU(BUFFER_VERTEX_NORMAL);
		UploadToGPU(BUFFER_VERTEX_UV);
	}

	void RenderContext::SetRenderTarget(
		const RenderTarget& renderTarget,
		const DepthBuffer& depthBuffer)
	{
		ID3D11DepthStencilView* pTemp = NULL;

		if (depthBuffer.pView)
		{
			pTemp = depthBuffer.pView;
		}

		// --- SET RENDER TARGETS ---
		m_pContext4->OMSetRenderTargets(
			1,
			&renderTarget.pView,
			pTemp);
	}

	void RenderContext::SetGraphicsPipeline(GraphicsPipeline* pPipeline)
	{
		// --- SET PIPELINE --- 
		if (m_pCurrentPipeline != pPipeline)
		{
			m_pCurrentPipeline = pPipeline;

			m_pContext4->IASetInputLayout(m_pCurrentPipeline->m_pLayout);

			m_pContext4->VSSetShader(
				m_pCurrentPipeline->m_pVertexShader,
				NULL,
				0);

			m_pContext4->PSSetShader(
				m_pCurrentPipeline->m_pPixelShader,
				NULL,
				0);
		}
	}

	void RenderContext::SetViewport(
		const UINT x,
		const UINT y,
		const UINT width,
		const UINT height)
	{
		m_viewport.TopLeftX = (FLOAT)x;
		m_viewport.TopLeftY = (FLOAT)y;

		m_viewport.Width	= (FLOAT)width;
		m_viewport.Height	= (FLOAT)height;

		m_pContext4->RSSetViewports(1, &m_viewport);
	}

	void RenderContext::VSSetConstantBuffer(
		const UINT slot,
		const BufferRegion& region)
	{
		ID3D11Buffer* pBuffer = m_pStorage->GetBufferHeapGPU(region.Type);

		UINT firstConstant	= region.DataLocation / 16;
		UINT numConstants	= region.DataCount / 16;

		m_pContext4->VSSetConstantBuffers1(
			slot,
			1,
			&pBuffer,
			&firstConstant,
			&numConstants);
	}

	void RenderContext::DrawInstanced(
		const UINT instanceCount,
		const UINT instanceStart,
		const BufferRegion& meshRegion)
	{
		UINT vertexCountPerInstance = meshRegion.DataCount / (sizeof(float) * 3);
		UINT startVertexLocation	= meshRegion.DataLocation / (sizeof(float) * 3);

		m_pContext4->DrawInstanced(
			vertexCountPerInstance,
			instanceCount,
			startVertexLocation,
			instanceStart);
	}

	void RenderContext::DrawIndexedInstance(
		const UINT instanceCount,
		const UINT instanceStart,
		const BufferRegion& indexRegion,
		const BufferRegion& meshRegion)
	{
		UINT indexCountPerInstance	= indexRegion.DataCount / sizeof(int);

		UINT startIndexLocation		= indexRegion.DataLocation / sizeof(int);
		UINT startVertexLocation	= meshRegion.DataLocation / (sizeof(float) * 3);

		m_pContext4->DrawIndexedInstanced(
			indexCountPerInstance,
			instanceCount,
			startIndexLocation,
			startVertexLocation,
			instanceStart);
	}
}
