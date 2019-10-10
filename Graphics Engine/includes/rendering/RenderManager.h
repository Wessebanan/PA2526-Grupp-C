#pragma once

#include "RenderTechnique.h"

#include "DefaultTechnique.h"
#include "SceenSpaceTechnique.h"
#include "TransformationTechnique.h"

#include "RenderCamera.h"

#include <DirectXMath.h>

namespace rendering
{
	struct SUN_DESC
	{
		DirectX::XMFLOAT3 Direction = { 0.0f, 0.0f, -1.0f };
		DirectX::XMFLOAT3 Position = { 0.0f, 0.0f, 0.0f };

		float
			Width = 1.0f,
			Height = 1.0f,
			NearPlane = 0.1f,
			FarPlane = 1.0f;

		UINT Resolution = 1;

		char Red, Green, Blue;
	};

	struct DirectionalLight
	{
		DirectX::XMFLOAT3 Direction;
		// Red Green Blue Illumination (1 Byte each)
		uint32_t packedData;
	};

	class RenderManager
	{
	public:
		RenderManager();
		~RenderManager();

		void Initialize(
			const SUN_DESC& sunDesc,
			const UINT clientWidth,
			const UINT clientHeight,
			const char* pTitle);

		void Clear(const float red, const float green, const float blue);
		void Draw();

		void Destroy();

		int CreateMesh(
			const VERTEX_BUFFER_DATA* pVertexData,
			const INDEX_BUFFER_DATA* pIndexData);

		void CreateModelHeap(
			const TECHNIQUE_HEAP_LAYOUT_DESC layoutDesc[RENDER_TECHNIQUES_COUNT]);

		void* GetTechniqueModelBuffer(
			const RENDER_TECHNIQUES techniqueIndex);

		void SetViewMatrix(const DirectX::XMFLOAT4X4 viewMatrix);

		graphics::PresentWindow* GetPresentWindow();

		template<RENDER_TECHNIQUES T>
		inline void* GetData()
		{
			return m_geometry.GetData<T>();
		}

	private:
		void DrawShadowMap();
		void DrawGeometry();

		graphics::DeviceInterface m_device;
		graphics::RenderContext* m_pContext;

		UINT m_clientWidth, m_clientHeight;

		RenderTechniques m_geometry;
		DrawManager m_drawManager;

		graphics::PresentWindow m_wnd;
		graphics::RenderTarget m_target;

		graphics::BufferRegion m_modelDataRegion;

		// Camera
		graphics::BufferRegion m_viewMatrixRegion;
		graphics::BufferRegion m_projectionMatrixRegion;
		graphics::DepthBuffer m_depthBuffer;

		// Sun
		DirectionalLight m_sunData;
		graphics::BufferRegion m_sunDataRegion;

		graphics::BufferRegion m_sunViewMatrix;
		graphics::BufferRegion m_sunProjMatrix;
		graphics::DepthBuffer m_shadowMap;


		char* m_pMemoryForTechniques;
	};
} 