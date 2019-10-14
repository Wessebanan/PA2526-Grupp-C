#pragma once

#pragma once

#include "RenderContext.h"

#include <vector>
#include <DirectXMath.h>

namespace graphics
{
	class GraphicsPipeline;

	struct ShaderModelLayout
	{
		UINT MeshCount;			// Total Meshes Within This Struct
		MeshRegion Meshes[2];		// Which Meshes To Be Drawn

		UINT InstanceCounts[2];	// Total Instances Per Mesh

		UINT TotalModels;
	};

	struct ShaderProgram
	{
		ID3D11VertexShader* pVertexShader;
		ID3D11PixelShader* pPixelShader;
		UINT PerObjectByteWidth;
	};

	struct RenderManagerData
	{
		ID3D11Texture2D* pBackBufferTexture;
	};

	class RenderManager
	{
	public:
		RenderManager();
		~RenderManager();

		HRESULT Initialize(const HWND hWnd);

		void Destroy();

		UINT CreateShaderProgram(
			const char* pVSFilepath,
			const char* pPSFilepath,
			const UINT perObjectByteWidth);

		UINT CreatePipeline(GraphicsPipeline* pPipeline, const void* pDescription);
		void UpdatePipeline(const UINT pipeline, const void* pPipelineData);
		void ExecutePipeline(const UINT pipeline);

		void SetModelData(const void* pData, const UINT byteWidth);
		void SetShaderModelLayout(const UINT shader, const ShaderModelLayout& rLayout);

		void Present();

	private:
		ID3D11Device4* m_pDevice4;
		ID3D11DeviceContext4* m_pContext4;

		IDXGIFactory6* m_pFactory6;
		IDXGIAdapter4* m_pAdapter4;

		IDXGISwapChain4* m_pSwapChain;

		ID3D11Buffer* m_pPerObjectBuffer;
		const void* m_pData;
		UINT m_dataByteWidth;

		UINT m_clientWidth, m_clientHeight;

		RenderManagerData m_data;

		std::vector<ShaderModelLayout> m_shaderModelLayouts;

		std::vector<ShaderProgram> m_shaderPrograms;
		std::vector<GraphicsPipeline*> m_pipelines;
	};
}