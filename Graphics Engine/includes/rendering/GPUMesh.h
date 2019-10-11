#pragma once

#include "RenderHeader.h"

namespace rendering
{
	class GPUMesh
	{
	public:
		void Draw(const UINT count, const UINT start, graphics::RenderContext* pContext);
		void SetMeshRegion(const graphics::BufferRegion& rMeshRegion);
		void SetIndexRegion(const graphics::BufferRegion& rIndexRegion);

	private:
		graphics::BufferRegion m_meshRegion;
		graphics::BufferRegion m_indexRegion;

	};

	inline void GPUMesh::Draw(const UINT count, const UINT start, graphics::RenderContext* pContext)
	{
		switch (m_indexRegion.DataCount)
		{
			// No indexed region
		case 0:
			pContext->DrawInstanced(
				count,
				start,
				m_meshRegion);
			break;

			// otherwise draw with indexed region
		default:
			pContext->DrawIndexedInstance(
				count,
				start,
				m_indexRegion,
				m_meshRegion);
			break;
		}
	}

	inline void GPUMesh::SetMeshRegion(const graphics::BufferRegion& rMeshRegion)
	{
		m_meshRegion = graphics::BufferRegion(rMeshRegion);
		m_indexRegion.DataCount = 0;
	}

	inline void GPUMesh::SetIndexRegion(const graphics::BufferRegion& rIndexRegion)
	{
		m_indexRegion = graphics::BufferRegion(rIndexRegion);
	}
}