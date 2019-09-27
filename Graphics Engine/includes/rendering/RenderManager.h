#pragma once

#include "RenderTechnique.h"

#include "DefaultTechnique.h"
#include "WaterTechnique.h"

namespace rendering
{
	class RenderManager
	{
	public:
		RenderManager();
		~RenderManager();

		void Initialize();

		void Draw();
		void Destroy();

		int LoadMeshes(
			const LOAD_MESH_DESC* pDesc,
			const UINT count);

		template<RENDER_TECHNIQUES T>
		inline void* GetData()
		{
			return m_techniques.GetData<T>();
		}

	private:
		graphics::DeviceInterface* m_pDevice;
		graphics::RenderContext* m_pContext;

		RenderTechniques m_techniques;

		MeshManager m_meshManager;
		DrawManager m_drawManager;

		graphics::BufferRegion m_meshDataRegion;

		char* m_pMemoryForTechniques;
	};
} 