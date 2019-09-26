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

		void Draw(
			RENDER_TECHNIQUES technique,
			graphics::RenderContext* pContext);

	private:
		MeshManager* m_pMeshManager;
	};
}