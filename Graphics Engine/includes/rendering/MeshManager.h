#pragma once

#include "RenderHeader.h"
#include <vector>

namespace rendering
{
	class GPUMesh
	{
	public:
		/*
			How much memory is used per model that uses this mesh
		*/
		UINT GetModelSize()
		{
			return m_modelByteWidth;
		}

		void Draw(
			const UINT count,
			const UINT start,
			graphics::RenderContext* pContext)
		{
			pContext->DrawIndexedInstance(
				count,
				start,
				m_meshRegion,
				m_indexRegion);
		}

		void SetMeshRegion(const graphics::BufferRegion& rMeshRegion)
		{
			m_meshRegion = graphics::BufferRegion(rMeshRegion);
		}

		void SetIndexRegion(const graphics::BufferRegion& rIndexRegion)
		{
			m_indexRegion = graphics::BufferRegion(rIndexRegion);
		}
	private:

		graphics::BufferRegion m_meshRegion;
		graphics::BufferRegion m_indexRegion;
		UINT m_modelByteWidth;

	};

	struct PerTechniqueData
	{
		/*
			A Technique can have several meshes to be drawn by it
			MeshCount value is how much of a specific mesh that needs to be drawn
			MeshCount index is which mesh it is
		*/
		std::vector<UINT> MeshCount;
	};

	struct LOAD_MESH_DESC
	{
		std::string Filepath;
		UINT InstanceCount;
		UINT ByteWidth;
		RENDER_TECHNIQUES Technique;
	};

	class MeshManager
	{
		friend class RenderManager;
		friend class DrawManager;

	public:
		MeshManager();
		~MeshManager();

		int LoadMeshes(
			graphics::DeviceInterface* pDevice,
			const LOAD_MESH_DESC* pDesc,
			const UINT count);

		void* AllocateMesh(const int index);

	private:
		PerTechniqueData m_perTechniqueData[RENDER_TECHNIQUES_COUNT];

		char* m_pModelData;
		UINT m_modelDataSize;

		char** m_ppModelDataStart; // Per Mesh

		GPUMesh* m_pMeshes;

		int LoadMeshFromFile(
			graphics::DeviceInterface* pDevice,
			const std::string filepath,
			const int index);
	};
}