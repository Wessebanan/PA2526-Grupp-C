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

		void Start(graphics::RenderContext* pContext);

		/*
			For each technique draw all instances to render to screen
		*/
		void Draw(
			RENDER_TECHNIQUES technique,
			graphics::RenderContext* pContext);

	private:
		MeshManager* m_pMeshManager;
		graphics::BufferRegion* m_pBuffer;

		// Region to hold everything to draw all meshes
		graphics::BufferRegion m_perObjectRegion;
	};
}