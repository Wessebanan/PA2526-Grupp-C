#include "../..//includes/rendering/MeshManager.h"

namespace rendering
{
	MeshManager::MeshManager()
	{
		m_modelDataSize = 0;
	}

	MeshManager::~MeshManager()
	{
		free(m_pModelData);
	}

	int MeshManager::LoadMeshes(
		graphics::DeviceInterface* pDevice,
		const LOAD_MESH_DESC* pDesc, 
		const UINT count)
	{
		m_modelDataSize = 0;
		for (UINT i = 0; i < count; i++)
		{
			if (pDesc[i].ByteWidth > graphics::RenderContext::CB_MAX_BYTES_PER_BIND) 
			{
				return FALSE;
			}

			m_modelDataSize += pDesc[i].ByteWidth * pDesc[i].InstanceCount;
			m_perTechniqueData[pDesc[i].Technique].MeshCount.push_back(pDesc[i].InstanceCount);
		}

		size_t maxAllocSize = 
			m_modelDataSize
			+ count * sizeof(GPUMesh) 
			+ count * sizeof(char*);

		char* pData = (char*)malloc(maxAllocSize);

		m_pModelData = pData;
		ZeroMemory(m_pModelData, m_modelDataSize);

		pData += m_modelDataSize;
		m_pMeshes = (GPUMesh*)pData;

		pData += count * sizeof(GPUMesh);
		m_ppModelDataStart = (char**)pData;

		UINT size = 0;
		for (UINT i = 0; i < count; i++)
		{
			size += pDesc[i].ByteWidth * pDesc[i].InstanceCount;
			m_ppModelDataStart[i] = m_pModelData + size;

			LoadMeshFromFile(pDevice, pDesc[i].Filepath, i);
		}

		return TRUE;
	}

	void* MeshManager::AllocateMesh(const int index)
	{
		void* pStart = m_ppModelDataStart[index];
		m_ppModelDataStart[index] += m_pMeshes[index].GetModelSize();

		return pStart;
	}

	int MeshManager::LoadMeshFromFile(
		graphics::DeviceInterface* pDevice, 
		const std::string filepath, 
		const int index)
	{
		return 0;
	}
}