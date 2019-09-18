#pragma once

#include "BufferStorage.h"
#include "PresentWindow.h"

namespace graphics
{
	enum SHADER_RESOURCE_SLOT
	{
		SHADER_RESOURCE_SHADOW_MAP = 16,
	};

	class RenderTarget
	{
		friend class RenderContext;
		friend class DeviceInterface;

		BufferRegion Region;
		ID3D11RenderTargetView* pView;
	};

	class DepthBuffer
	{
		friend class RenderContext;
		friend class DeviceInterface;

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

		void UploadStaticDataToGPU();
		void UploadDynamicDataToGPU();
		void UploadMeshesToGPU();

	private:
		void UploadToGPU(const BUFFER_TYPE type);

		void Initialize(ID3D11Device4* pDevice4, InternalStorage* pStorage);
		void Release();

		float m_clearColor[4];

		InternalStorage* m_pStorage;
		ID3D11DeviceContext4* m_pContext4;
		GraphicsPipeline* m_pCurrentPipeline;

		D3D11_VIEWPORT m_viewport;
	};


	// --- DEVICE INTERFACE ---

	class DeviceInterface
	{

	public:
		DeviceInterface();
		~DeviceInterface();

		RenderContext* GetRenderContext();
		UINT64 QueryVideoMemoryUsage();

		void CreatePresentWindow(
			const UINT width,
			const UINT height,
			const char* pTitle,
			RenderTarget* pRenderTarget,
			DepthBuffer* pDepthBuffer,
			PresentWindow** ppWindow);

		void CreatePipeline(
			const std::string& vertexShader,
			const std::string& pixelShader,
			GraphicsPipeline** ppPipeline);

		void CreateDynamicBufferRegion(
			const UINT size,
			const void* pData,
			BufferRegion* pRegion);

		void CreateStaticBufferRegion(
			const UINT size,
			const void* pData,
			BufferRegion* pRegion);

		void CreateIndexBufferRegion(
			const UINT size,
			const void* pData,
			BufferRegion* pRegion);

		bool CreateMeshRegion(
			const UINT vertexCount,
			const void* pVertices,
			const void* pNormals,
			const void* pUVs,
			BufferRegion* pRegion);

		void DeletePipeline(GraphicsPipeline* pPipeline);
		void DeleteRenderTarget(const RenderTarget& renderTarget);
		void DeleteDepthBuffer(const DepthBuffer& depthBuffer);


		void Release();

	private:
		void Initialize();
		void CreateBufferRegion(
			const BUFFER_TYPE type,
			const UINT size,
			BufferRegion* pRegion);

		friend void CreateDeviceInterface(DeviceInterface** ppDevice);

		ID3D11Device4* m_pDevice4;
		IDXGIFactory6* m_pFactory6;
		IDXGIAdapter4* m_pAdapter4;

		PresentWindow m_window;
		RenderContext m_context;

		GraphicsPipelineArray m_pipelineArray;
		InternalStorage m_storage;
	};

	inline void CreateDeviceInterface(DeviceInterface** ppDevice)
	{
		DeviceInterface* pObj = new DeviceInterface();
		pObj->Initialize();

		(*ppDevice) = pObj;
	}
}