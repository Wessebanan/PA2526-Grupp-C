#pragma once

#include <iostream>
#include "MeshManager.h"

namespace rendering
{
	class DrawManager
	{
		friend class RenderManager;

	public:
		DrawManager();
		~DrawManager();

		void Clear();

		void Draw(
			RENDER_TECHNIQUES technique,
			graphics::RenderContext* pContext);

	private:
		MeshManager* m_pMeshManager;
		graphics::BufferRegion* m_pBuffer;

		// Region to hold everything to draw all meshes
		graphics::BufferRegion m_perObjectRegion;

		UINT m_start;
	};
}