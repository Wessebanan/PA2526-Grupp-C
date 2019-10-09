//#pragma once
//
//#include "GPUMesh.h"
//
//namespace rendering
//{
//	struct PerTechniqueData
//	{
//		/*
//			A Technique can have several meshes to be drawn by it
//			MeshCount value is how much of a specific mesh that needs to be drawn
//			MeshCount index is which mesh it is
//		*/
//		std::vector<UINT> MeshCount;
//		std::vector<UINT> MeshIndex;
//
//		UINT PerInstanceByteWidth;
//
//		char* pModelDataStart;
//		UINT ModelDataByteWidth;
//
//		UINT TotalModelCount;
//	};
//
//	struct MODEL_LAYOUT_DESC
//	{
//		UINT MeshIndex;			// What mesh to use
//		UINT InstanceCount;		// How many instances of that mesh
//	};
//
//	struct TECHNIQUE_HEAP_LAYOUT_DESC
//	{
//		UINT PerInstanceByteWidth;		// How much data per instance
//
//		MODEL_LAYOUT_DESC* pModelLayout;
//		UINT ModelLayoutCount;
//	};
//
//	struct VERTEX_BUFFER_DATA
//	{
//		UINT VertexCount;
//	
//		void
//			*pVertexData,
//			*pNormalsData,
//			*pTextureCoordData,
//			*pBlendWeight,
//			*pBlendIndices;
//	};
//	
//	struct INDEX_BUFFER_DATA
//	{
//		UINT IndexCount;
//		void* pIndexData;
//	};
//
//	class MeshManager
//	{
//		friend class RenderManager;
//		friend class DrawManager;
//
//	public:
//		MeshManager();
//		~MeshManager();
//
//		int CreateMesh(
//			const VERTEX_BUFFER_DATA* pVertexData,
//			const INDEX_BUFFER_DATA* pIndexData,
//			graphics::DeviceInterface* pDevice);
//
//		int CreateModelHeap(const TECHNIQUE_HEAP_LAYOUT_DESC layoutDesc[RENDER_TECHNIQUES_COUNT]);
//
//		void* GetTechniqueModelBuffer(
//			const RENDER_TECHNIQUES techniqueIndex);
//
//	private:
//		PerTechniqueData m_perTechniqueData[RENDER_TECHNIQUES_COUNT];
//
//		char* m_pPerInstanceData;
//		UINT m_perInstanceDataSize;
//
//		std::vector<GPUMesh> m_meshes;
//	};
//}