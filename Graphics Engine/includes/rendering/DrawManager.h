#pragma once

#include "GPUMesh.h"
#include <vector>

namespace rendering
{
	struct PerTechniqueData
	{
		/*
			A Technique can have several meshes to be drawn by it
			MeshCount value is how much of a specific mesh that needs to be drawn
			MeshCount index is which mesh it is
		*/
		std::vector<UINT> MeshCount;
		std::vector<UINT> MeshIndex;

		UINT PerInstanceByteWidth;

		char* pModelDataStart;
		UINT ModelDataByteWidth;

		UINT TotalModelCount;
	};

	struct MODEL_LAYOUT_DESC
	{
		UINT MeshIndex;			// What mesh to use
		UINT InstanceCount;		// How many instances of that mesh
	};

	struct TECHNIQUE_HEAP_LAYOUT_DESC
	{
		UINT PerInstanceByteWidth;		// How much data per instance

		MODEL_LAYOUT_DESC* pModelLayout;
		UINT ModelLayoutCount;
	};

	struct VERTEX_BUFFER_DATA
	{
		UINT VertexCount;

		void
			* pVertexData,
			* pNormalsData,
			* pTextureCoordData,
			* pBlendWeight,
			* pBlendIndices;
	};

	struct INDEX_BUFFER_DATA
	{
		UINT IndexCount;
		void* pIndexData;
	};

	class DrawManager
	{
	public:
		DrawManager();
		~DrawManager();

		void Begin(graphics::RenderContext* pContext);

		/*
			Reset to start so it can be called again
		*/
		void Reset(graphics::RenderContext* pContext);

		/*
			For each technique draw all instances to render to screen
		*/
		void Draw(
			RENDER_TECHNIQUES technique,
			graphics::RenderContext* pContext);

		int CreateMesh(
			const VERTEX_BUFFER_DATA* pVertexData,
			const INDEX_BUFFER_DATA* pIndexData,
			graphics::DeviceInterface* pDevice);

		int CreateModelHeap(
			const TECHNIQUE_HEAP_LAYOUT_DESC layoutDesc[RENDER_TECHNIQUES_COUNT],
			graphics::DeviceInterface* pDevice);

		void* GetTechniqueModelBuffer(
			const RENDER_TECHNIQUES techniqueIndex);

	private:

		// Pointer To Buffer Where All Per Object Data Is Located
		graphics::BufferRegion m_buffer;

		// Region To Modify During Draw Loop To Match Data Offset
		graphics::BufferRegion m_perObjectRegion;


		PerTechniqueData m_perTechniqueData[RENDER_TECHNIQUES_COUNT];

		char* m_pPerInstanceData;
		UINT m_perInstanceDataSize;

		std::vector<GPUMesh> m_meshes;
	};
}