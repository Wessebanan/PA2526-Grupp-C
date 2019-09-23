#pragma once
//
//#include "DeviceInterface.h"
//
//#define MAXIMUM_RENDER_OBJECT_SIZE (3840)
//
//#include <DirectXMath.h>
//using namespace DirectX;
//
//namespace graphics
//{
//	class Renderer
//	{
//	public:
//		Renderer();
//		~Renderer();
//
//		void Initialize(const UINT width, const UINT height, const char* pTitle);
//
//		void Clear();
//		void Submit();
//		void Draw();
//		void Present();
//
//		void Release();
//
//	private:
//		DeviceInterface* m_pDevice;
//		GraphicsPipeline* m_pPipeline;
//		PresentWindow* m_pWindow;
//
//		RenderContext* m_pContext;
//
//		RenderTarget m_backBuffer;
//		DepthBuffer m_depthBuffer;
//
//		BufferRegion m_projRegion;
//		BufferRegion m_viewRegion;
//		BufferRegion m_buffer0;
//
//		UINT m_drawSize;
//
//		XMFLOAT4X4 m_view;
//		XMFLOAT4X4 m_projection;
//	};
//}