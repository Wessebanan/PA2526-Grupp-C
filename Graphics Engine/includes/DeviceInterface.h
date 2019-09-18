#pragma once

#include "BufferStorage.h"
#include "PresentWindow.h"

namespace graphics
{
	enum SHADER_RESOURCE_SLOT
	{
		SHADER_RESOURCE_SHADOW_MAP = 16,
	};


	// --- RENDER CONTEXT ---

	class RenderContext
	{
		static constexpr UINT CONSTANT_BUFFER_MAX_BIND_BYTE = 65536;
		friend class DeviceInterface;

	public:
		RenderContext();
		~RenderContext();

		void ClearRenderTarget(
			Texture2DView* pView,
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

		void SetRenderTarget(Texture2DView* pView);

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

		void UploadToGPU(const BUFFER_TYPE type);
		void UploadMeshesToGPU();

	private:

		void Initialize(ID3D11Device4* pDevice4, InternalStorage* pStorage);
		void Release();

		float m_clearColor[4];

		InternalStorage* m_pStorage;
		ID3D11DeviceContext4* m_pContext4;
		GraphicsPipeline* m_pCurrentPipeline;

		//Temporary fix for sprint goal
		ID3D11DepthStencilView* m_pDepthBuffer; 

		D3D11_VIEWPORT m_viewport;
	};


	// --- DEVICE INTERFACE ---

	class DeviceInterface
	{

	public:
		DeviceInterface();
		~DeviceInterface();

		void CreatePresentWindow(
			const UINT width,
			const UINT height,
			const char* pTitle,
			PresentWindow** ppWindow);

		RenderContext* QueryRenderContext();
		Texture2DView* QueryBackBuffer();

		void CreatePipeline(
			const std::string& vertexShader,
			const std::string& pixelShader,
			GraphicsPipeline** ppPipeline);

		void CreateDynamicBufferRegion(
			const UINT size,
			BufferRegion* pRegion);

		void CreateStaticBufferRegion(
			const UINT size,
			BufferRegion* pRegion);

		void CreateIndexBufferRegion(
			const UINT size,
			BufferRegion* pRegion);

		bool CreateMeshRegion(
			const UINT vertexCount,
			const void* pVertices,
			const void* pNormals,
			const void* pUVs,
			BufferRegion* pRegion);

		void DeletePipeline(GraphicsPipeline* pPipeline);

		UINT64 QueryVideoMemoryUsage();

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

		Texture2DView m_backBuffer;

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