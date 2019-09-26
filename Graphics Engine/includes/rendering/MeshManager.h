#pragma once

#include "RenderHeader.h"
#include <vector>

namespace rendering
{
	class MeshManager
	{
		friend class RenderManager;
		friend class DrawManager;

	public:
		MeshManager();
		~MeshManager();

		void LoadMesh();
		void GetMesh();

	private:
		graphics::DeviceInterface* m_pDevice;

		// Index of this struct will be the technique
		struct MeshInfo
		{
			UINT MeshCount;
			UINT MeshIndex;
		};

		UINT* m_pMeshesPerShader;
		MeshInfo* m_pMeshInfo;
	};
}