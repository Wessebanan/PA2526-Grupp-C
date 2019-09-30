#include "../..//includes/rendering/DrawManager.h"

namespace rendering
{
	DrawManager::DrawManager()
	{
	}

	DrawManager::~DrawManager()
	{
	}

	void DrawManager::Clear()
	{
		m_perObjectRegion = graphics::BufferRegion(*m_pBuffer);
		m_perObjectRegion.DataCount = graphics::RenderContext::CB_MAX_BYTES_PER_BIND;
	}

	void DrawManager::Draw(
		RENDER_TECHNIQUES technique, 
		graphics::RenderContext* pContext)
	{
		UINT techniqueCount = technique;
		PerTechniqueData& ptd = m_pMeshManager->m_perTechniqueData[techniqueCount];

		UINT start = 0;
		UINT maxBytesPerBind		= graphics::RenderContext::CB_MAX_BYTES_PER_BIND;
		UINT maxMeshes				= (UINT)ptd.MeshCount.size();
		UINT byteWidth				= ptd.PerInstanceByteWidth;

		for (UINT atMesh = 0; atMesh < maxMeshes; atMesh++)
		{
			UINT maxDrawCountPerDraw = m_perObjectRegion.DataCount / byteWidth;

			UINT meshIndex		= ptd.MeshIndex[atMesh];
			UINT maxMeshCount	= ptd.MeshCount[atMesh];
			GPUMesh* pMesh		= &m_pMeshManager->m_meshes[meshIndex];

			UINT toDraw = maxMeshCount;
			while (toDraw > 0)
			{
				UINT drawCount = toDraw;
				if (drawCount + start > maxDrawCountPerDraw)
					drawCount = maxDrawCountPerDraw - start;

				pContext->VSSetConstantBuffer(0, m_perObjectRegion);
				pMesh->Draw(drawCount, start, pContext);

				toDraw -= drawCount;
				start	+= drawCount;

				if (start >= maxDrawCountPerDraw)
				{
					m_perObjectRegion.DataLocation += maxDrawCountPerDraw * ptd.PerInstanceByteWidth;
					start = 0;
				}
			}
		}

		// Align the location to prepare for the next technique
		m_perObjectRegion.DataLocation = (UINT)ceil(m_perObjectRegion.DataLocation / 256.0f) * 256;
	}
}