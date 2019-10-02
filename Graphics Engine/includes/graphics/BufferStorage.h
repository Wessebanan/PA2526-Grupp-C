#pragma once

#include "Window.h"
#include "GraphicsPipeline.h"

namespace graphics
{
	enum TEXTURE2D_TYPE
	{
		TEXTURE2D_SHADER_RESOURCE,
		TEXUTRE2D_RENDER_TARGET,
		TEXTURE2D_DEPTH_STENCIL,
	};

	//struct ResourceView
	//{
	//	friend class RenderContext;
	//	friend class DeviceInterface;

	//private:
	//	TEXTURE2D_TYPE Type;

	//	union
	//	{
	//		// use with depth stencil
	//		ID3D11Texture2D* pTexture;	

	//		// use with render targets and shader resources
	//		BufferRegion Region;		
	//	};		

	//	union
	//	{
	//		ID3D11RenderTargetView* pRenderTargetView;
	//		ID3D11ShaderResourceView* pShaderResourceView;
	//		ID3D11DepthStencilView* pDepthStencilView;
	//	};
	//};

	enum BUFFER_TYPE
	{
		BUFFER_RENDER_TARGET,		// RENDER TARGETS
		BUFFER_SHADER_RESOURCE,		// SHADER RESOURCES

		BUFFER_CONSTANT_DYNAMIC,	// CONSTANT BUFFER DYNAMIC
		BUFFER_CONSTANT_STATIC,		// CONSTANT BUFFER DEFAULT

		BUFFER_VERTEX_INDICES,		// INDEX BUFFER
		BUFFER_VERTEX_POSITION,		// VERTEX BUFFER POSITIONS
		BUFFER_VERTEX_NORMAL,		// VERTEX BUFFER NORMALS
		BUFFER_VERTEX_UV,			// VERTEX BUFFER UVS
		BUFFER_VERTEX_ARRAY_INDEX,	// VERTEX BUFFER ARRAY INDEX

		BUFFER_TYPE_LENGTH,			// LENGTH OF BUFFER_TYPE
		BUFFER_TYPE_UNKNOWN,		// FOR OBJECTS THAT DON'T NEED TO HANDLE UPLOADS
	};

	struct BufferRegion
	{
		UINT
			DataCount,
			DataLocation,
			ID;

		BUFFER_TYPE Type;
	};

#define ALLOCATED_BUFFER_COUNT		(BUFFER_TYPE_LENGTH)
#define ALLOCATED_BUFFER_SIZE		(65536 * 4)
#define ALLOCATED_RTV_AND_SRV_SIZE	(1474560)
#define TOTAL_ALLOCATED_CPU_BTYES	(ALLOCATED_RTV_AND_SRV_SIZE * 2 + ALLOCATED_BUFFER_SIZE * (ALLOCATED_BUFFER_COUNT - 2))

	class BufferHeap
	{
		friend class InternalStorage;
		friend class RenderContext;
		friend class DeviceInterface;

	public:
		BufferHeap();
		~BufferHeap();

		int Initialize(
			ID3D11Device4* pDevice4,
			BUFFER_TYPE type,
			char** pDataStart);

		int AllocateRegion(const UINT size, BufferRegion* pRegion);
		void CopyDataToRegion(const void* pData, const BufferRegion& region);
		void UploadToGPU(ID3D11DeviceContext4* pContext4, const BUFFER_TYPE type);

		void Release();

	private:
		ID3D11Buffer* m_pBuffer;
		D3D11_BIND_FLAG m_bindType;
		BUFFER_TYPE m_type;

		char* m_pCpuData;
		UINT m_currentSpent;
		UINT m_size;
		UINT m_currentCreated;
	};

	class InternalStorage
	{
		friend class RenderContext;

	public:
		InternalStorage();
		~InternalStorage();

		int Initialize(ID3D11Device4* pDevice4);
		BufferHeap* GetBufferHeapCPU(const BUFFER_TYPE type);
		ID3D11Buffer* GetBufferHeapGPU(const BUFFER_TYPE type);

		void Release();

	private:
		BufferHeap m_heaps[ALLOCATED_BUFFER_COUNT];
		char m_cpuData[TOTAL_ALLOCATED_CPU_BTYES];
	};
}