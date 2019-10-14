#include "../..//includes/rendering/DrawManager.h"

namespace rendering
{
	DrawManager::DrawManager()
	{
	}
	DrawManager::~DrawManager()
	{
		free(m_pPerInstanceData);
	}

	void DrawManager::Begin(graphics::RenderContext* pContext)
	{
		pContext->CopyDataToRegion(
			m_pPerInstanceData,
			m_perInstanceDataSize,
			m_buffer);
	}

	void DrawManager::Reset(graphics::RenderContext* pContext)
	{
		m_perObjectRegion = graphics::BufferRegion(m_buffer);

		m_perObjectRegion.DataCount = graphics::RenderContext::CB_MAX_BYTES_PER_BIND;
		pContext->VSSetConstantBuffer(0, m_perObjectRegion);
	}

	void DrawManager::Draw(
		RENDER_TECHNIQUES technique, 
		graphics::RenderContext* pContext)
	{
		UINT techniqueCount = technique;
		PerTechniqueData& ptd = m_perTechniqueData[techniqueCount];

		UINT start					= 0;
		UINT maxBytesPerBind		= graphics::RenderContext::CB_MAX_BYTES_PER_BIND;
		UINT maxMeshes				= (UINT)ptd.MeshCount.size();
		UINT byteWidth				= ptd.PerInstanceByteWidth;

		for (UINT atMesh = 0; atMesh < maxMeshes; atMesh++)
		{
			UINT maxDrawCountPerDraw = m_perObjectRegion.DataCount / byteWidth;

			// Get Mesh Related Data
			UINT meshIndex		= ptd.MeshIndex[atMesh];
			UINT maxMeshCount	= ptd.MeshCount[atMesh];
			GPUMesh* pMesh		= &m_meshes[meshIndex];

			// Loop till every instance is drawn
			UINT toDraw = maxMeshCount;
			while (toDraw > 0)
			{
				// How many instances to draw
				UINT drawCount = toDraw;

				// if there are too many instances, draw with max possible this pass
				if (drawCount + start > maxDrawCountPerDraw)
					drawCount = maxDrawCountPerDraw - start;

				// Execute draw
				pMesh->Draw(drawCount, start, pContext);

				// Decrement with drawn instances
				toDraw -= drawCount;

				// set new start array index for shader to read from
				start	+= drawCount;
				
				// when end of shader array index 
				if (start >= maxDrawCountPerDraw)
				{
					// increment data pointer in shader
					m_perObjectRegion.DataLocation +=
						maxDrawCountPerDraw * ptd.PerInstanceByteWidth;
					pContext->VSSetConstantBuffer(0, m_perObjectRegion);

					// reset shader array index 
					start = 0;
				}
			}
		}

		/*
			Since every technique has different size for model data,
			the data start for the a technique has to be aligned to
			a standard 256 byte alignment (set by DirectX). 
		
			With this alignment the data start pointer has to be incremented
			for every next technique to align where the following
			technique data start.

			t1			   t2            t3
			|data|data|da  |data|data|d  |dat 
		*/

		m_perObjectRegion.DataLocation +=
			start * ptd.PerInstanceByteWidth;

		// Align the location to prepare for the next technique
		m_perObjectRegion.DataLocation = (UINT)ceil((m_perObjectRegion.DataLocation) / 256.0f) * 256;

		pContext->VSSetConstantBuffer(0, m_perObjectRegion);
	}

	int DrawManager::CreateMesh(
		const VERTEX_BUFFER_DATA* pVertexData, 
		const INDEX_BUFFER_DATA* pIndexData, 
		graphics::DeviceInterface* pDevice)
	{
		// If there's no data to create a mesh
		if (!pVertexData) return -1;

		GPUMesh mesh;

		graphics::BufferRegion meshRegion;

		// Create vertex buffer
		int result = pDevice->CreateVertexBufferRegion(
			pVertexData->VertexCount,
			pVertexData->pVertexData,
			pVertexData->pNormalsData,
			pVertexData->pTextureCoordData,
			pVertexData->pBlendWeight,
			pVertexData->pBlendIndices,
			&meshRegion);

		if (!result) return -1;

		mesh.SetMeshRegion(meshRegion);

		// if there's index data, create a region for that too
		if (pIndexData)
		{
			graphics::BufferRegion indexRegion;

			pDevice->CreateIndexBufferRegion(
				pIndexData->IndexCount,
				pIndexData->pIndexData,
				&indexRegion);

			mesh.SetIndexRegion(indexRegion);
		}

		m_meshes.push_back(mesh);

		return (UINT)m_meshes.size() - 1;
	}

	int DrawManager::CreateModelHeap(
		const TECHNIQUE_HEAP_LAYOUT_DESC layoutDesc[RENDER_TECHNIQUES_COUNT],
		graphics::DeviceInterface* pDevice)
	{
		UINT totalByteWidth = 0;
		// Per Technique
		for (UINT t = 0; t < RENDER_TECHNIQUES_COUNT; t++)
		{
			PerTechniqueData& ptd = m_perTechniqueData[t];
			ptd.PerInstanceByteWidth = layoutDesc[t].PerInstanceByteWidth;

			UINT modelCount = 0;
			// Per mesh inside technique
			for (UINT m = 0; m < layoutDesc[t].ModelLayoutCount; m++)
			{
				// Store data for draw manager
				ptd.MeshCount.push_back(layoutDesc[t].pModelLayout[m].InstanceCount);
				ptd.MeshIndex.push_back(layoutDesc[t].pModelLayout[m].MeshIndex);

				modelCount += layoutDesc[t].pModelLayout[m].InstanceCount;
			}

			// Calculate the amount of memory needed for a technique
			ptd.TotalModelCount = modelCount;
			ptd.ModelDataByteWidth = ptd.PerInstanceByteWidth * modelCount;
			totalByteWidth += (UINT)ceil(ptd.ModelDataByteWidth / 256.0f) * 256;
		}

		// Allocate data and zero memory
		m_pPerInstanceData = (char*)malloc(totalByteWidth);
		m_perInstanceDataSize = totalByteWidth;
		ZeroMemory(m_pPerInstanceData, m_perInstanceDataSize);

		// Set memory slot start per technique
		char* pDataBegin = m_pPerInstanceData;
		for (UINT i = 0; i < RENDER_TECHNIQUES_COUNT; i++)
		{
			PerTechniqueData& ptd = m_perTechniqueData[i];

			if (ptd.ModelDataByteWidth > 0)
				ptd.pModelDataStart = pDataBegin;
			else
				ptd.pModelDataStart = NULL;

			pDataBegin += (UINT)(ceil(ptd.ModelDataByteWidth / 256.0f) * 256);
		}

		// Create Region on GPU for the model data
		pDevice->CreateDynamicBufferRegion(
			m_perInstanceDataSize, 
			NULL, 
			&m_buffer);

		return 0;
	}

	void* DrawManager::GetTechniqueModelBuffer(const RENDER_TECHNIQUES techniqueIndex)
	{
		PerTechniqueData& ptd = m_perTechniqueData[techniqueIndex];
		return ptd.pModelDataStart;
	}
}