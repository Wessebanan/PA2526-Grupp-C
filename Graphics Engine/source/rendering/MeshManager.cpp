//#include "../..//includes/rendering/MeshManager.h"
//
//namespace rendering
//{
//	MeshManager::MeshManager()
//	{
//		m_perInstanceDataSize = 0;
//		m_pPerInstanceData = NULL;
//	}
//
//	MeshManager::~MeshManager()
//	{
//		free(m_pPerInstanceData);
//	}
//
//	int MeshManager::CreateMesh(
//		const VERTEX_BUFFER_DATA* pVertexData, 
//		const INDEX_BUFFER_DATA* pIndexData, 
//		graphics::DeviceInterface* pDevice)
//	{
//		// If there's no data to create a mesh
//		if (!pVertexData) return -1;
//
//		GPUMesh mesh;
//
//		graphics::BufferRegion meshRegion;
//		
//		// Create vertex buffer
//		int result = pDevice->CreateVertexBufferRegion(
//			pVertexData->VertexCount,
//			pVertexData->pVertexData,
//			pVertexData->pNormalsData,
//			pVertexData->pTextureCoordData,
//			pVertexData->pBlendWeight,
//			pVertexData->pBlendIndices,
//			&meshRegion);
//		
//		if (!result) return -1;
//
//		mesh.SetMeshRegion(meshRegion);
//		
//		// if there's index data, create a region for that too
//		if (pIndexData)
//		{
//			graphics::BufferRegion indexRegion;
//
//			pDevice->CreateIndexBufferRegion(
//				pIndexData->IndexCount,
//				pIndexData->pIndexData,
//				&indexRegion);
//
//			mesh.SetIndexRegion(indexRegion);
//		}
//
//		m_meshes.push_back(mesh);
//		
//		return (UINT)m_meshes.size() - 1;
//	}
//
//	int MeshManager::CreateModelHeap(
//		const TECHNIQUE_HEAP_LAYOUT_DESC layoutDesc[RENDER_TECHNIQUES_COUNT])
//	{
//		UINT totalByteWidth = 0;
//		// Per Technique
//		for (UINT t = 0; t < RENDER_TECHNIQUES_COUNT; t++)
//		{
//			PerTechniqueData& ptd = m_perTechniqueData[t];
//			ptd.PerInstanceByteWidth = layoutDesc[t].PerInstanceByteWidth;
//
//			UINT modelCount = 0;
//			// Per mesh inside technique
//			for (UINT m = 0; m < layoutDesc[t].ModelLayoutCount; m++)
//			{
//				// Store data for draw manager
//				ptd.MeshCount.push_back(layoutDesc[t].pModelLayout[m].InstanceCount);
//				ptd.MeshIndex.push_back(layoutDesc[t].pModelLayout[m].MeshIndex);
//
//				modelCount += layoutDesc[t].pModelLayout[m].InstanceCount;
//			}
//
//			// Calculate the amount of memory needed for a technique
//			ptd.TotalModelCount = modelCount;
//			ptd.ModelDataByteWidth = ptd.PerInstanceByteWidth * modelCount;
//			totalByteWidth += (UINT)ceil(ptd.ModelDataByteWidth / 256.0f) * 256;
//		}
//
//		// Allocate data and zero memory
//		m_pPerInstanceData = (char*)malloc(totalByteWidth);
//		m_perInstanceDataSize = totalByteWidth;
//		ZeroMemory(m_pPerInstanceData, m_perInstanceDataSize);
//
//		// Set memory slot start per technique
//		char* pDataBegin = m_pPerInstanceData;
//		for (UINT i = 0; i < RENDER_TECHNIQUES_COUNT; i++)
//		{
//			PerTechniqueData& ptd = m_perTechniqueData[i];
//
//			if(ptd.ModelDataByteWidth > 0)
//				ptd.pModelDataStart = pDataBegin;
//			else
//				ptd.pModelDataStart = NULL;
//
//			pDataBegin += (UINT)(ceil(ptd.ModelDataByteWidth / 256.0f) * 256);
//		}
//
//		return 0;
//	}
//
//	void* MeshManager::GetTechniqueModelBuffer(
//		const RENDER_TECHNIQUES techniqueIndex)
//	{
//		PerTechniqueData& ptd = m_perTechniqueData[techniqueIndex];
//		return ptd.pModelDataStart;
//	}
//}