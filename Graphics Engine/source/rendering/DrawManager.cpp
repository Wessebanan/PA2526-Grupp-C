#include "../..//includes/rendering/DrawManager.h"

namespace rendering
{
	DrawManager::DrawManager()
	{
	}

	DrawManager::~DrawManager()
	{
	}

	void DrawManager::Draw(
		RENDER_TECHNIQUES technique, 
		graphics::RenderContext* pContext)
	{
		//// --- DRAW MANAGER ---
		//UINT meshIndex = 0;
		//std::map<UINT, UINT> meshCount = mrData.m_meshCount;
		//while (at < end && meshIndex < 2)
		//{
		//	r.DataLocation = at;							// Set active region (65536 bytes per draw)
		//	pContext->VSSetConstantBuffer(0, r);			// Apply active Region

		//	UINT drawn = 0;
		//	while (drawn < 1024 && meshIndex < meshCount.size())
		//	{
		//		UINT count = meshCount[meshIndex];

		//		if (count + drawn > 1024)
		//		{
		//			count = 1024 - drawn;
		//		}

		//		meshCount[meshIndex] -= count;

		//		pContext->DrawInstanced(count, drawn, meshes[meshIndex]);		// Draw (with mesh[1])
		//		drawn += count;

		//		if (meshCount[meshIndex] == 0)
		//		{
		//			meshIndex++;
		//		}
		//	}

		//	at += RenderContext::CB_MAX_BYTES_PER_BIND;									// Increment active region
		//}

		UINT i = 0;

		UINT meshCount = m_pMeshManager->m_pMeshInfo[technique + i].MeshCount;
	}
}