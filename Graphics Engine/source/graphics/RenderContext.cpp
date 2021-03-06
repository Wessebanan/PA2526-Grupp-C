#include "../../includes/graphics/DeviceInterface.h"
#include <DirectXMath.h>

namespace graphics
{
	using namespace DirectX;

	RenderContext::RenderContext()
	{
		m_viewport = { 0 };
		m_viewport.MaxDepth = 1.0f;
	}

	RenderContext::~RenderContext()
	{
	}

	ID3D11DeviceContext4* RenderContext::GetInternalContext()
	{
		return m_pContext4;
	}

	int RenderContext::Initialize(ID3D11Device4* pDevice4, InternalStorage* pStorage)
	{
		m_pStorage = pStorage;

		// Set Vertex Buffers

		ID3D11Buffer* pVertexBuffers[6] = {
			m_pStorage->GetBufferHeapGPU(BUFFER_VERTEX_POSITION),
			m_pStorage->GetBufferHeapGPU(BUFFER_VERTEX_NORMAL),
			m_pStorage->GetBufferHeapGPU(BUFFER_VERTEX_UV),

			m_pStorage->GetBufferHeapGPU(BUFFER_VERTEX_ARRAY_INDEX),

			m_pStorage->GetBufferHeapGPU(BUFFER_VERTEX_BLEND_WEIGHT),
			m_pStorage->GetBufferHeapGPU(BUFFER_VERTEX_BLEND_INDICES),
		};

		UINT strides[6] = {
			sizeof(float)	* 3,
			sizeof(float)	* 3,
			sizeof(float)	* 2,

			sizeof(UINT)	* 1,

			sizeof(float)	* 3,
			sizeof(int)		* 4,
		};

		UINT offsets[6] = {
			0,
			0,
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
			m_pStorage->GetBufferHeapGPU(BUFFER_VERTEX_INDICES),
			DXGI_FORMAT_R32_UINT,
			0);

		return TRUE;
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
		float clear_color[4];

		clear_color[0] = red;
		clear_color[1] = green;
		clear_color[2] = blue;
		clear_color[3] = 1.0f;

		m_pContext4->ClearRenderTargetView(
			renderTarget.pView,
			clear_color);
	}

	void RenderContext::CopyDataToRegion(
		const void* pData,
		const UINT byteWidth,
		const BufferRegion& region)
	{
		BufferHeap* pHeap = m_pStorage->GetBufferHeapCPU(region.Type);
		pHeap->CopyDataToRegion(pData, region);
	}

	void RenderContext::internal_UploadBufferToGPU(const BUFFER_TYPE type)
	{
		BufferHeap* pHeap = m_pStorage->GetBufferHeapCPU(type);
		pHeap->UploadToGPU(m_pContext4, type);
	}

	void RenderContext::UploadBufferToGPU(const BUFFER_UPLOAD_TYPE type)
	{
		switch (type)
		{
		case BUFFER_UPLOAD_STATIC_DATA:
			internal_UploadBufferToGPU(BUFFER_CONSTANT_STATIC);
			break;
		case BUFFER_UPLOAD_DYNAMIC_DATA:
			internal_UploadBufferToGPU(BUFFER_CONSTANT_DYNAMIC);
			break;
		case BUFFER_UPLOAD_INDEX_DATA:
			internal_UploadBufferToGPU(BUFFER_VERTEX_INDICES);
			break;
		case BUFFER_UPLOAD_VERTEX_DATA:
			internal_UploadBufferToGPU(BUFFER_VERTEX_POSITION);
			internal_UploadBufferToGPU(BUFFER_VERTEX_NORMAL);
			internal_UploadBufferToGPU(BUFFER_VERTEX_UV);
			internal_UploadBufferToGPU(BUFFER_VERTEX_BLEND_WEIGHT);
			internal_UploadBufferToGPU(BUFFER_VERTEX_BLEND_INDICES);
			break;
		default:
			break;
		}
	}

	void RenderContext::SetRenderTarget(
		const RenderTarget* pRenderTarget,
		const DepthBuffer* pDepthBuffer)
	{
		ID3D11RenderTargetView* pTarget = NULL;
		ID3D11DepthStencilView* pDepth = NULL;

		if (pRenderTarget)
			pTarget = pRenderTarget->pView;

		if (pDepthBuffer)
			pDepth = pDepthBuffer->pView;

		// --- SET RENDER TARGETS ---
		m_pContext4->OMSetRenderTargets(
			1,
			&pTarget,
			pDepth);
	}

	void RenderContext::DiscardPixelShader()
	{
		m_pContext4->PSSetShader(NULL, NULL, 0);
	}

	void RenderContext::SetGraphicsPipeline(GraphicsPipeline* pPipeline)
	{
		m_pContext4->IASetInputLayout(pPipeline->m_pLayout);
		m_pContext4->VSSetShader(pPipeline->m_pVertexShader, NULL, 0);
		m_pContext4->PSSetShader(pPipeline->m_pPixelShader, NULL, 0);
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

		UINT firstConstant	= region.DataLocation	/ 16;
		UINT numConstants	= region.DataCount		/ 16;

		m_pContext4->VSSetConstantBuffers1(
			slot,
			1,
			&pBuffer,
			&firstConstant,
			&numConstants);
	}

	void RenderContext::PSSetConstantBuffer(
		const UINT slot,
		const BufferRegion& region)
	{
		ID3D11Buffer* pBuffer = m_pStorage->GetBufferHeapGPU(region.Type);

		UINT firstConstant = region.DataLocation / 16;
		UINT numConstants = region.DataCount / 16;

		m_pContext4->PSSetConstantBuffers1(
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
