#include "../../pch.h"
#include "../includes/RenderManager.h"
#include "../includes/Pipeline.h"

#define MAX_BYTES_PER_DRAW 131072

namespace graphics
{
	RenderManager::RenderManager()
	{
	}

	RenderManager::~RenderManager()
	{
	}

	HRESULT RenderManager::Initialize(const HWND hWnd)
	{
		HRESULT hr = S_OK;

		m_clientWidth	= window::GetClientResolution(hWnd).x;
		m_clientHeight	= window::GetClientResolution(hWnd).y;

		hr = graphics::internal::InitializeD3D11();

		graphics::internal::D3D11_DEVICE_HANDLE handle;
		graphics::internal::GetD3D11(&handle);
		m_pDevice4	= handle.pDevice4;
		m_pContext4 = handle.pDeviceContext4;
		m_pFactory6	= handle.pFactory6;
		m_pAdapter4	= handle.pAdapter4;

		if (FAILED(hr)) return hr;

		hr = graphics::CreateSwapChain(
			m_pDevice4,
			m_pFactory6,
			hWnd,
			&m_pSwapChain);

		if (FAILED(hr)) return hr;

		hr = m_pSwapChain->GetBuffer(
			0, 
			IID_PPV_ARGS(&m_data.pBackBufferTexture));

		if (FAILED(hr)) return hr;

		// Per Model Buffer
		{
			D3D11_BUFFER_DESC desc = { 0 };
			desc.BindFlags		= D3D11_BIND_CONSTANT_BUFFER;
			desc.ByteWidth		= MAX_BYTES_PER_DRAW;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			desc.Usage			= D3D11_USAGE_DYNAMIC;

			hr = m_pDevice4->CreateBuffer(&desc, NULL, &m_pPerObjectBuffer);
		}

		return FAILED(hr) ? hr : S_OK;
	}

	void RenderManager::Destroy()
	{
		for (UINT i = 0; i < m_shaderPrograms.size(); i++)
		{
			m_shaderPrograms[i].pVertexShader->Release();
			m_shaderPrograms[i].pPixelShader->Release();
		}

		for (UINT i = 0; i < m_pipelines.size(); i++)
		{
			m_pipelines[i]->Delete();
		}

		m_pPerObjectBuffer->Release();

		m_data.pBackBufferTexture->Release();
		m_pSwapChain->Release();

		graphics::internal::DestroyD3D11();
	}

	UINT RenderManager::CreatePipeline(GraphicsPipeline* pPipeline, const void* pDescription)
	{
		HRESULT hr = pPipeline->Initialize(m_pDevice4, pDescription);
		if (FAILED(hr)) return UINT_MAX;

		m_pipelines.push_back(pPipeline);
		return m_pipelines.size() - 1;
	}

	UINT RenderManager::CreateShaderProgram(
		const char* pVSFilepath,
		const char* pPSFilepath,
		const UINT perObjectByteWidth)
	{
		HRESULT hr;
		graphics::RenderProgram program;

		hr = graphics::CreateVertexShaderFromFile(m_pDevice4, pVSFilepath, &program.pVertexShader);
		if (FAILED(hr)) return UINT_MAX;
		
		graphics::CreatePixelShaderFromFile(m_pDevice4, pPSFilepath, &program.pPixelShader);
		if (FAILED(hr)) return UINT_MAX;
		
		program.PerObjectByteWidth = perObjectByteWidth;

		m_shaderPrograms.push_back(program);
		m_shaderModelLayouts.push_back(ShaderModelLayout());

		return m_shaderPrograms.size() - 1;
	}

	void RenderManager::SetModelData(const void* pData, const UINT byteWidth)
	{
		m_pData = pData;
		m_dataByteWidth = byteWidth;
	}

	void RenderManager::SetShaderModelLayout(const UINT shader, const ShaderModelLayout& rLayout)
	{
		m_shaderModelLayouts[shader] = rLayout;
	}

	void RenderManager::UpdatePipeline(const UINT pipeline, const void* pPipelineData)
	{
		m_pipelines[pipeline]->Update(m_pContext4, pPipelineData);
	}

	void RenderManager::ExecutePipeline(const UINT pipeline)
	{
		GraphicsPipeline* pPipeline = m_pipelines[pipeline];

		pPipeline->Begin(m_pContext4);

		graphics::UploadToDynamicBuffer(
			m_pContext4,
			m_pPerObjectBuffer,
			m_pData,
			m_dataByteWidth,
			0);

		UINT data_location = 0;
		UINT shader_count = (UINT)m_shaderPrograms.size();
		for (UINT i = 0; i < shader_count; i++)
		{
			ShaderModelLayout& layout = m_shaderModelLayouts[i];

			pPipeline->PreExecute(
				m_pContext4, 
				m_shaderPrograms[i].pVertexShader, 
				m_shaderPrograms[i].pPixelShader);

			graphics::DrawMeshes(
				m_pContext4,
				m_pPerObjectBuffer,
				data_location,
				m_shaderPrograms[i].PerObjectByteWidth,
				layout.MeshCount,
				layout.Meshes,
				layout.InstanceCounts);

			data_location += layout.TotalModels * m_shaderPrograms[i].PerObjectByteWidth;
			data_location = (UINT)ceil(data_location / 256.0f) * 256;
		}

		pPipeline->End(m_pContext4, &m_data);
	}

	void RenderManager::Present()
	{
		m_pSwapChain->Present(1, 0);
	}
}