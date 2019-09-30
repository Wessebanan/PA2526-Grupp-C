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

		void Clear();
		void Draw();

		void Destroy();

		int CreateMesh(
			const VERTEX_BUFFER_DATA* pVertexData,
			const INDEX_BUFFER_DATA* pIndexData);

		void CreateModelHeap(
			const TECHNIQUE_HEAP_LAYOUT_DESC layoutDesc[RENDER_TECHNIQUES_COUNT]);

		void* GetTechniqueModelBuffer(
			const RENDER_TECHNIQUES techniqueIndex);

		graphics::PresentWindow* SetWndSettings(
			const UINT width, 
			const UINT height, 
			const char* pTitle);

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

		graphics::PresentWindow m_wnd;
		graphics::RenderTarget m_target;
		graphics::DepthBuffer m_depthBuffer;

		graphics::BufferRegion m_meshDataRegion;

		char* m_pMemoryForTechniques;
	};
} 