#pragma once

#include "GraphicsInterface.h"
#include "Pipeline.h"

#include <vector>

namespace graphics
{
	struct ComputeProgram
	{
		ID3D11ComputeShader* pComputeShader;
	};

	class ComputeManager
	{
	public:
		ComputeManager();
		~ComputeManager();

		HRESULT Initialize();

		UINT CreateShaderProgram(const char* pCSFilepath);

		UINT CreatePipeline(ComputePipeline* pPipeline, const void* pDescription);
		void UpdatePipeline(const UINT pipeline, const void* pPipelineData);
		void ExecutePipeline(const UINT pipeline, const UINT program);

		void Destroy();

	private:
		std::vector<ComputeProgram> m_programs;
		std::vector<ComputePipeline*> m_pipelines;

		ID3D11Device4* m_pDevice4;
		ID3D11DeviceContext4* m_pContext4;

		IDXGIFactory6* m_pFactory6;
		IDXGIAdapter4* m_pAdapter4;
	};
}