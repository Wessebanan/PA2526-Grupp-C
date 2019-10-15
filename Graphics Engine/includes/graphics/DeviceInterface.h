#pragma once

#include "BufferStorage.h"
#include "PresentWindow.h"

namespace graphics
{
	enum BUFFER_UPLOAD_TYPE
	{
		BUFFER_UPLOAD_VERTEX_DATA,
		BUFFER_UPLOAD_STATIC_DATA,
		BUFFER_UPLOAD_DYNAMIC_DATA,
		BUFFER_UPLOAD_INDEX_DATA
	};

	struct RenderTarget
	{
		friend class RenderContext;
		friend class DeviceInterface;

	private:
		BufferRegion Region;
		ID3D11RenderTargetView* pView;
	};

	struct DepthBuffer
	{
		friend class RenderContext;
		friend class DeviceInterface;

		ID3D11ShaderResourceView* pResource;
	private:
		ID3D11DepthStencilView* pView;
	};

	// --- RENDER CONTEXT ---

	class RenderContext
	{
		friend class DeviceInterface;

	public:
		static constexpr UINT CB_MAX_BYTES_PER_BIND = 65536;

		RenderContext();
		~RenderContext();

		ID3D11DeviceContext4* GetInternalContext();

		void ClearDepth(const DepthBuffer& depthBuffer);

		void ClearRenderTarget(
			const RenderTarget& renderTarget,
			const float red,
			const float green,
			const float blue);

		void SetGraphicsPipeline(GraphicsPipeline* pPipeline);

		void SetViewport(
			const UINT x,
			const UINT y,
			const UINT width,
			const UINT height);

		void VSSetConstantBuffer(
			const UINT slot,
			const BufferRegion& region);

		template <UINT N>
		void VSSetConstantBuffers(
			const UINT startSlot,
			const BufferRegion* pRegions)
		{
			ID3D11Buffer* pBuffers[N];
			UINT first_constants[N];
			UINT num_constants[N];

			for (UINT i = 0; i < N; i++)
			{
				pBuffers[i] = m_pStorage->GetBufferHeapGPU(pRegions[i].Type);

				first_constants[i]	= pRegions[i].DataLocation / 16;
				num_constants[i]	= pRegions[i].DataCount / 16;
			}

			m_pContext4->VSSetConstantBuffers1(
				startSlot,
				N,
				pBuffers,
				first_constants,
				num_constants);
		}

		void PSSetConstantBuffer(
			const UINT slot,
			const BufferRegion& region);

		template <UINT N>
		void PSSetConstantBuffers(
			const UINT startSlot,
			const BufferRegion* pRegions)
		{
			ID3D11Buffer* pBuffers[N];
			UINT first_constants[N];
			UINT num_constants[N];

			for (UINT i = 0; i < N; i++)
			{
				pBuffers[i] = m_pStorage->GetBufferHeapGPU(pRegions[i].Type);

				first_constants[i] = pRegions[i].DataLocation / 16;
				num_constants[i] = pRegions[i].DataCount / 16;
			}

			m_pContext4->PSSetConstantBuffers1(
				startSlot,
				N,
				pBuffers,
				first_constants,
				num_constants);
		}

		void SetRenderTarget(
			const RenderTarget* pRenderTarget, 
			const DepthBuffer* pDepthBuffer);

		void DiscardPixelShader();

		void DrawInstanced(
			const UINT instanceCount,
			const UINT instanceStart,
			const BufferRegion& meshRegion);

		void DrawIndexedInstance(
			const UINT instanceCount,
			const UINT instanceStart,
			const BufferRegion& indexRegion,
			const BufferRegion& meshRegion);

		void CopyDataToRegion(
			const void* pData,
			const UINT byteWidth,
			const BufferRegion& region);

		void UploadBufferToGPU(const BUFFER_UPLOAD_TYPE type);

	private:
		void internal_UploadBufferToGPU(const BUFFER_TYPE type);

		int Initialize(ID3D11Device4* pDevice4, InternalStorage* pStorage);
		void Release();

		ID3D11DeviceContext4* m_pContext4;
		D3D11_VIEWPORT m_viewport;

		InternalStorage* m_pStorage;
	};


	// --- DEVICE INTERFACE ---

	class DeviceInterface
	{

	public:
		DeviceInterface();
		~DeviceInterface();

		int Initialize();
		void Release();

		RenderContext* GetRenderContext();

		int CreatePresentWindow(
			const UINT width,
			const UINT height,
			const char* pTitle,
			RenderTarget* pRenderTarget,
			PresentWindow* pWindow);

		int CreateDepthBuffer(
			const UINT width,
			const UINT height,
			DepthBuffer* pDepthBuffer,
			const bool createShaderResource);

		int CreateGraphicsPipeline(
			const std::string& vertexShader,
			const std::string& pixelShader,
			GraphicsPipeline* pPipeline);

		int CreateDynamicBufferRegion(
			const UINT size,
			const void* pData,
			BufferRegion* pRegion);

		int CreateStaticBufferRegion(
			const UINT size,
			const void* pData,
			BufferRegion* pRegion);

		int CreateIndexBufferRegion(
			const UINT indexCount,
			const void* pData,
			BufferRegion* pRegion);

		int CreateVertexBufferRegion(
			const UINT vertexCount,
			const void* pVertices,
			const void* pNormals,
			const void* pUVs,
			const void* pBlendWeights,
			const void* pBlendIndices,
			BufferRegion* pRegion);

		void DeleteGraphicsPipeline(GraphicsPipeline* pPipeline);
		void DeleteRenderTarget(const RenderTarget& renderTarget);
		void DeleteDepthBuffer(const DepthBuffer& depthBuffer);
		void DeletePresentWindow(const PresentWindow& window);

	private:
		int CreateBufferRegion(
			const BUFFER_TYPE type,
			const UINT size,
			const void* pData,
			BufferRegion* pRegion);

		ID3D11Device4* m_pDevice4;
		IDXGIFactory6* m_pFactory6;
		IDXGIAdapter4* m_pAdapter4;

		RenderContext m_context;

		// handled in the class because it will be easier 
		// for render context to fetch with some kind of index
		InternalStorage m_storage;

		// Sampler states will only be a definite amount 
		ID3D11SamplerState* m_pSamplerStates[10];
	};
}