#pragma once

#include "DeviceInterface.h"

#define MAXIMUM_DRAW_LIST_SIZE (10)
#define MAXIMUM_MESHES (10)
#define MAXIMUM_SHADERS (10)

#include <DirectXMath.h>
using namespace DirectX;

namespace graphics
{
	struct VERTEX_BUFFER_DATA
	{
		UINT VertexCount;

		void
			*pVertexData,
			*pNormalsData,
			*pTextureCoordData;
	};

	struct INDEX_BUFFER_DATA
	{
		UINT IndexCount;
		void* pIndexData;
	};

	struct GPUMesh
	{
		int BufferIndex,
			IsIndexed;
	};

	struct GPUShader
	{
		int Index;
	};

	struct PerObjectData
	{
		UINT byteWidth;
		void* pData;
	};

	struct Model
	{
		GPUMesh GPUMeshIndex;
		GPUShader GPUShaderIndex;
	};

	class Renderer
	{
	public:
		Renderer();
		~Renderer();

		void Initialize(
			const UINT width, 
			const UINT height, 
			const char* pTitle);
		/*
		Creates mesh with specified input data.
		
		Required:
		- Vertex Data			/float3
		
		Optional:
		- Normal Data			/float3
		- Texture Coord Data	/float2
		- Index Data			/uint
		*/
		int CreateMesh(
			const VERTEX_BUFFER_DATA* pVertexData,
			const INDEX_BUFFER_DATA* pIndexData,
			GPUMesh& rMesh);

		void Clear();
		void Submit(
			const Model& rModel,
			const PerObjectData& rData);

		void Draw();

		void Release();

		PresentWindow* GetWindow();

	private:
		char* m_pAt;
		char m_perObjectData[65536];

		DeviceInterface* m_pDevice;
		RenderContext* m_pContext;

		RenderTarget m_backBuffer;
		DepthBuffer m_depthBuffer;
		PresentWindow m_window;

		BufferRegion m_projRegion;
		BufferRegion m_viewRegion;
		BufferRegion m_buffer0;

		BufferRegion m_vertexBuffers[MAXIMUM_MESHES];
		BufferRegion m_indexBuffers[MAXIMUM_MESHES];
		UINT m_meshCount;

		GraphicsPipeline* m_pPipelines[MAXIMUM_SHADERS];

		Model m_drawList[MAXIMUM_DRAW_LIST_SIZE];
		UINT m_drawCount;

		XMFLOAT4X4 m_view;
		XMFLOAT4X4 m_projection;
	};
}