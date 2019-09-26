#pragma once

#include "RenderTechnique.h"

#include "DefaultTechnique.h"
#include "WaterTechnique.h"

namespace rendering
{
	struct Model
	{
		void* pData;
		UINT ByteWidth;

		UINT MeshID;
	};

	class RenderManager
	{
	public:
		RenderManager();
		~RenderManager();

		void Initialize();
		void Submit(const Model& rModel);
		void Draw();
		void Destroy();

		MeshManager* pGetMeshManager();

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
	};
}