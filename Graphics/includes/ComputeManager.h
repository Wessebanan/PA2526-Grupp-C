#pragma once

#include "RenderContext.h"

#include <vector>

namespace graphics
{	
	class ComputePipeline;

	/*
		A Shader Program Used For Computing
	*/
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

		/*
			Create a shader program to be called on ExecutePipeline()
			: Returns UINT_MAX if failed
		*/
		UINT CreateShaderProgram(const char* pCSFilepath);

		/*
			Create pipeline (Every pipeline has their own structs for description)
			: Returns UINT_MAX if failed
		*/
		UINT CreatePipeline(ComputePipeline* pPipeline, const void* pDescription);

		/*
			Update pipeline data (Every pipeline has their own structs for data)
		*/
		void UpdatePipeline(const UINT pipeline, const void* pPipelineData);

		/*
			Execute desired pipeline
		*/
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