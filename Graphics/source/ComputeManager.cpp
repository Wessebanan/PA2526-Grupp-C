#include "../../pch.h"
#include "../includes/ComputeManager.h"
#include "../includes/Pipeline.h"

namespace graphics
{
	ComputeManager::ComputeManager()
	{
	}

	ComputeManager::~ComputeManager()
	{
	}

	HRESULT ComputeManager::Initialize()
	{
		HRESULT hr = S_OK;

		hr = graphics::internal::InitializeD3D11();

		graphics::internal::D3D11_DEVICE_HANDLE handle;
		graphics::internal::GetD3D11(&handle);
		m_pDevice4	= handle.pDevice4;
		m_pContext4 = handle.pDeviceContext4;
		m_pFactory6 = handle.pFactory6;
		m_pAdapter4 = handle.pAdapter4;

		return hr;
	}

	UINT ComputeManager::CreateShaderProgram(const char* pCSFilepath)
	{
		HRESULT hr;
		graphics::ComputeProgram program;

		hr = graphics::CreateComputeShaderFromFile(m_pDevice4, pCSFilepath, &program.pComputeShader);
		if (FAILED(hr)) return UINT_MAX;
		
		m_programs.push_back(program);

		return (UINT)m_programs.size() - 1;
	}

	UINT ComputeManager::CreatePipeline(ComputePipeline* pPipeline, const void* pDescription)
	{
		HRESULT hr = pPipeline->Initialize(m_pDevice4, pDescription);
		if (FAILED(hr)) return UINT_MAX;

		m_pipelines.push_back(pPipeline);
		return (UINT)m_pipelines.size() - 1;
	}

	void ComputeManager::UpdatePipeline(const UINT pipeline, const void* pPipelineData)
	{
		m_pipelines[pipeline]->Update(m_pContext4, pPipelineData);
	}

	void ComputeManager::ExecutePipeline(const UINT pipeline, const UINT program)
	{
		m_pipelines[pipeline]->Execute(m_pContext4, m_programs[program].pComputeShader);
	}

	void ComputeManager::Destroy()
	{
		for (UINT i = 0; i < m_programs.size(); i++)
		{
			m_programs[i].pComputeShader->Release();
		}

		for (UINT i = 0; i < m_pipelines.size(); i++)
		{
			m_pipelines[i]->Delete();
		}

		graphics::internal::DestroyD3D11();
	}
}