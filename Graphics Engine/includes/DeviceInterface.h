#pragma once

#include "BufferStorage.h"
#include "PresentWindow.h"

namespace graphics
{
	enum SHADER_RESOURCE_SLOT
	{
		SHADER_RESOURCE_SHADOW_MAP = 16,
	};

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

		void SetRenderTarget(
			const RenderTarget& renderTarget, 
			const DepthBuffer& depthBuffer);

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

		float m_clearColor[4];

		ID3D11DeviceContext4* m_pContext4;
		D3D11_VIEWPORT m_viewport;

		InternalStorage* m_pStorage;
		GraphicsPipeline* m_pCurrentPipeline;
	};


	// --- DEVICE INTERFACE ---

	class DeviceInterface
	{

	public:
		DeviceInterface();
		~DeviceInterface();

		RenderContext* GetRenderContext();
		UINT64 QueryVideoMemoryUsage();

		int CreatePresentWindow(
			const UINT width,
			const UINT height,
			const char* pTitle,
			RenderTarget* pRenderTarget,
			PresentWindow* pWindow);

		int CreateDepthBuffer(
			const UINT width,
			const UINT height,
			DepthBuffer* pDepthBuffer);

		int CreateGraphicsPipeline(
			const std::string& vertexShader,
			const std::string& pixelShader,
			GraphicsPipeline** ppPipeline);

		int CreateDynamicBufferRegion(
			const UINT size,
			const void* pData,
			BufferRegion* pRegion);

		int CreateStaticBufferRegion(
			const UINT size,
			const void* pData,
			BufferRegion* pRegion);

		int CreateIndexBufferRegion(
			const UINT size,
			const void* pData,
			BufferRegion* pRegion);

		int CreateVertexBufferRegion(
			const UINT vertexCount,
			const void* pVertices,
			const void* pNormals,
			const void* pUVs,
			BufferRegion* pRegion);

		void DeleteGraphicsPipeline(GraphicsPipeline* pPipeline);
		void DeleteRenderTarget(const RenderTarget& renderTarget);
		void DeleteDepthBuffer(const DepthBuffer& depthBuffer);
		void DeletePresentWindow(const PresentWindow& window);

	private:
		void Release();
		int Initialize();
		int CreateBufferRegion(
			const BUFFER_TYPE type,
			const UINT size,
			const void* pData,
			BufferRegion* pRegion);

		friend int CreateDeviceInterface(DeviceInterface** ppDevice);
		friend void DeleteDeviceInterface(DeviceInterface* pDevice);

		ID3D11Device4* m_pDevice4;
		IDXGIFactory6* m_pFactory6;
		IDXGIAdapter4* m_pAdapter4;

		RenderContext m_context;

		// Internal storage because it will be easier 
		// for render context to fetch with index
		GraphicsPipelineArray m_pipelineArray;
		InternalStorage m_storage;

		// Sampler states will only be a definite amount 
		ID3D11SamplerState* m_pSamplerStates[10];
	};

	inline int CreateDeviceInterface(DeviceInterface** ppDevice)
	{
		DeviceInterface* pObj = new DeviceInterface();
		int result = pObj->Initialize();
		(*ppDevice) = pObj;

		return result;
	}

	inline void DeleteDeviceInterface(DeviceInterface* pDevice)
	{
		pDevice->Release();
		delete pDevice;
	}
}