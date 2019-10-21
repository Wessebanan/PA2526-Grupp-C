#pragma once

#pragma once

#include "RenderContext.h"

#include <vector>
#include <DirectXMath.h>

namespace graphics
{
	class GraphicsPipeline;

	/*
		Describes The Model Layout For A Render Program
	*/
	struct ShaderModelLayout
	{
		UINT MeshCount;				// Total Meshes Within This Struct
		MeshRegion Meshes[10];		// Which Meshes To Be Drawn

		UINT InstanceCounts[10];	// Total Instances Per Mesh

		UINT TotalModels;
	};

	/*
		A Shader Program Used For Rendering
	*/
	struct RenderProgram
	{
		ID3D11VertexShader* pVertexShader;
		ID3D11PixelShader* pPixelShader;
		UINT PerObjectByteWidth;
	};

	/*
		Internal Data Structure Used For Communicating Between Pipelines
	*/
	struct RenderManagerData
	{
		ID3D11Texture2D* pBackBufferTexture;
	};

	class RenderManager
	{
	public:
		RenderManager();
		~RenderManager();

		/*
			Initialize with a window that will be the back buffer
		*/
		HRESULT Initialize(const HWND hWnd);

		/*
			Create a shader program to be called on every ExecutePipeline()
			: Returns UINT_MAX if failed
		*/
		UINT CreateShaderProgram(
			const char* pVSFilepath,
			const char* pPSFilepath,
			const UINT perObjectByteWidth);

		/*
			Create Pipeline (Every pipeline has their own structs for description)
			: Returns UINT_MAX if failed
		*/
		UINT CreatePipeline(GraphicsPipeline* pPipeline, const void* pDescription);

		/*
			Update pipeline data (Every pipeline has their own structs for data)
		*/
		void UpdatePipeline(const UINT pipeline, const void* pPipelineData);

		/*
			Execute desired pipeline (will use all created programs)
		*/
		void ExecutePipeline(const UINT pipeline);

		/*
			Set the model data for all pipelines in one go (Must be update when data is changed)
		*/
		void SetModelData(const void* pData, const UINT byteWidth);

		/*
			Set shader model layout for a shader (Can be set once or every frame)
		*/
		void SetShaderModelLayout(const UINT shader, const ShaderModelLayout& rLayout);

		/*
			Preset back buffer
		*/
		void Present();

		void Destroy();
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

		std::vector<RenderProgram> m_shaderPrograms;
		std::vector<GraphicsPipeline*> m_pipelines;
	};
}