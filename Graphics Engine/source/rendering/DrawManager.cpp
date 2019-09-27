#include "../..//includes/rendering/DrawManager.h"

namespace rendering
{
	DrawManager::DrawManager()
	{
		m_perObjectRegion.DataCount = graphics::RenderContext::CB_MAX_BYTES_PER_BIND;
	}

	DrawManager::~DrawManager()
	{
	}

	void DrawManager::Clear()
	{
		m_perObjectRegion.DataLocation = m_pBuffer->DataLocation;
		m_start = 0;
	}

	void DrawManager::Draw(
		RENDER_TECHNIQUES technique, 
		graphics::RenderContext* pContext)
	{
		UINT techniqueCount = technique;
		PerTechniqueData m = m_pMeshManager->m_perTechniqueData[techniqueCount];

		UINT maxMeshes	= (UINT)m.MeshCount.size();
		UINT meshIndex	= 0;

		while (meshIndex < maxMeshes)
		{
			GPUMesh* pMesh		= &m_pMeshManager->m_pMeshes[meshIndex];
			UINT maxMeshCount	= m.MeshCount[meshIndex];
			UINT byteWidth		= pMesh->GetModelSize();

			UINT maxMeshCountPerDraw = m_perObjectRegion.DataCount / byteWidth;

			UINT numDrawCalls = (maxMeshCount / maxMeshCountPerDraw) + 1;

			for (UINT i = 0; i < numDrawCalls; i++)
			{
				UINT toDraw = maxMeshCount % maxMeshCountPerDraw;

				pContext->VSSetConstantBuffer(0, m_perObjectRegion);
				pMesh->Draw(toDraw, m_start, pContext);

				m_start += toDraw;
				m_perObjectRegion.DataLocation += graphics::RenderContext::CB_MAX_BYTES_PER_BIND;
			}

			meshIndex++;
		}
	}
}