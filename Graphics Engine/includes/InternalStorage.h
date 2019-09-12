#pragma once

#include "Window.h"
#include "Texture2DView.h"
#include "GraphicsPipeline.h"
#include "Mesh.h"

namespace graphics
{
	enum BUFFER_TYPE
	{
		BUFFER_CONSTANT_DYNAMIC,	// CONSTANT BUFFER DYNAMIC
		BUFFER_CONSTANT_STATIC,		// CONSTANT BUFFER DEFAULT
		BUFFER_RTV,					// RENDER TARGETS
		BUFFER_SRV,					// SHADER RESOURCES
		BUFFER_INDEX,				// INDEX BUFFER
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

#define ALLOCATED_BUFFERS (BUFFER_TYPE_LENGTH)
#define ALLOCATED_BUFFER_SIZE (65536)

	class BufferHeap
	{
		friend class InternalStorage;
		friend class RenderContext;
		friend class DeviceInterface;

	public:
		BufferHeap();
		~BufferHeap();

		void Initialize(
			ID3D11Device4* pDevice4,
			BUFFER_TYPE type);

		bool AllocateRegion(const UINT size, BufferRegion* pRegion);
		void CopyDataToRegion(const void* pData, const BufferRegion& region);
		void UploadToGPU(ID3D11DeviceContext4* pContext4, const BUFFER_TYPE type);

		void Release();

	private:
		ID3D11Buffer* m_pBuffer;
		D3D11_BIND_FLAG m_bindType;
		BUFFER_TYPE m_type;

		char m_cpuData[ALLOCATED_BUFFER_SIZE];
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

		void Initialize(ID3D11Device4* pDevice4);
		BufferHeap* GetBufferHeapCPU(const BUFFER_TYPE type);
		ID3D11Buffer* GetBufferHeapGPU(const BUFFER_TYPE type);

		void Release();

	private:
		BufferHeap m_heaps[ALLOCATED_BUFFERS];
	};
}