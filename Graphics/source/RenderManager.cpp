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

	HRESULT RenderManager::Initialize(
		const UINT clientWidth, 
		const UINT clientHeight, 
		const HWND hWnd)
	{
		HRESULT hr = S_OK;

		m_clientWidth = clientWidth;
		m_clientHeight = clientHeight;

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

		hr = m_meshManager.Initialize(m_pDevice4, 3000, 3000);

		if (FAILED(hr)) return hr;

		m_meshManager.EnableVertexBuffers(m_pContext4, 0);

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
		m_meshManager.Destroy();

		for (UINT i = 0; i < m_shaderPrograms.size(); i++)
		{
			m_shaderPrograms[i].pVertexShader->Release();
			m_shaderPrograms[i].pPixelShader->Release();
		}

		for (UINT i = 0; i < m_pipelines.size(); i++)
		{
			m_pipelines[i]->Destroy();
			m_pipelines[i]->Delete();
		}

		m_pPerObjectBuffer->Release();

		m_data.pBackBufferTexture->Release();
		m_pSwapChain->Release();

		graphics::internal::DestroyD3D11();
	}

	UINT RenderManager::CreateGraphicsPipeline(GraphicsPipeline* pPipeline, const void* pDescription)
	{
		pPipeline->Initialize(m_pDevice4, pDescription);
		m_pipelines.push_back(pPipeline);
		return m_pipelines.size() - 1;
	}

	UINT RenderManager::CreateGraphicsShaderProgram(
		const char* pVSFilepath,
		const char* pPSFilepath,
		const UINT perObjectByteWidth)
	{
		graphics::ShaderProgram program;

		graphics::CreateVertexShaderFromFile(m_pDevice4, pVSFilepath, &program.pVertexShader);
		graphics::CreatePixelShaderFromFile(m_pDevice4, pPSFilepath, &program.pPixelShader);
		program.PerObjectByteWidth = perObjectByteWidth;

		m_shaderPrograms.push_back(program);
		m_shaderModelLayouts.push_back(ShaderModelLayout());

		return m_shaderPrograms.size() - 1;
	}

	MeshRegion RenderManager::CreateMeshRegion(const UINT vertexCount, const UINT indexCount)
	{
		return m_meshManager.CreateMeshRegion(vertexCount, indexCount);
	}

	int RenderManager::UploadMeshData(
		const MeshRegion& meshRegion,
		const VERTEX_DATA& vertexData,
		const void* pIndices)
	{
		m_meshManager.UploadData(m_pContext4, meshRegion, vertexData, pIndices);
		return 0;
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

	void RenderManager::UpdateGraphicsPipelineData(const UINT pipeline, const void* pPipelineData)
	{
		m_pipelines[pipeline]->Update(m_pContext4, pPipelineData);
	}

	void RenderManager::ExecuteGraphicsPipeline(const UINT pipeline)
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

			pPipeline->Execute(
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