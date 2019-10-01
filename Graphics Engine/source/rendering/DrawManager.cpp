#include "../..//includes/rendering/DrawManager.h"

namespace rendering
{
	DrawManager::DrawManager()
	{
	}

	DrawManager::~DrawManager()
	{
	}

	void DrawManager::Start(graphics::RenderContext* pContext)
	{
		m_perObjectRegion = graphics::BufferRegion(*m_pBuffer);
		m_perObjectRegion.DataCount = graphics::RenderContext::CB_MAX_BYTES_PER_BIND;
		pContext->VSSetConstantBuffer(0, m_perObjectRegion);
	}

	void DrawManager::Draw(
		RENDER_TECHNIQUES technique, 
		graphics::RenderContext* pContext)
	{
		UINT techniqueCount = technique;
		PerTechniqueData& ptd = m_pMeshManager->m_perTechniqueData[techniqueCount];

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
			GPUMesh* pMesh		= &m_pMeshManager->m_meshes[meshIndex];

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
}