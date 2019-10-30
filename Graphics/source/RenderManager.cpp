
#include "../includes/RenderManager.h"

namespace graphics
{
	RenderManager::RenderManager()
	{
		m_clientWidth = 0;
		m_clientHeight = 0;

		m_firstTimeUpload = TRUE;
	}

	RenderManager::~RenderManager()
	{
	}

	HRESULT RenderManager::Initialize(const UINT totalBytesPerExecute)
	{
		HRESULT hr = S_OK;

		graphics::internal::D3D11_DEVICE_HANDLE handle;
		graphics::internal::GetD3D11(&handle);
		m_pDevice4	= handle.pDevice4;
		m_pContext4 = handle.pDeviceContext4;

		// Per Model Buffer
		{
			UINT bytes = totalBytesPerExecute;
			bytes = (UINT)(ceil(bytes / 256.f) * 256);

			if (bytes > 0)
			{

				D3D11_BUFFER_DESC desc = { 0 };
				desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
				desc.ByteWidth = bytes;
				desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
				desc.Usage = D3D11_USAGE_DYNAMIC;

				hr = m_pDevice4->CreateBuffer(&desc, NULL, &m_pPerObjectBuffer);
			}
		}

		return FAILED(hr) ? hr : S_OK;
	}

	UINT RenderManager::GetNumShaderPrograms()
	{
		return (UINT)m_shaders.size();
	}

	UINT RenderManager::GetNumPipelines()
	{
		return (UINT)m_pipelines.size();
	}

	void RenderManager::Destroy()
	{
		for (UINT i = 0; i < m_shaders.size(); i++)
		{
			m_shaders[i].Program.pVertexShader->Release();
			m_shaders[i].Program.pPixelShader->Release();
		}

		for (UINT i = 0; i < m_pipelines.size(); i++)
		{
			m_pipelines[i]->Delete();
		}

		SafeRelease((IUnknown**)&m_pPerObjectBuffer);
	}

	UINT RenderManager::CreatePipeline(GraphicsPipeline* pPipeline, const void* pDescription)
	{
		HRESULT hr = pPipeline->Initialize(m_pDevice4, pDescription);
		if (FAILED(hr)) return UINT_MAX;

		m_pipelines.push_back(pPipeline);
		return (UINT)m_pipelines.size() - 1;
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

		m_shaders.push_back({ShaderModelLayout(), program});

		return (UINT)m_shaders.size() - 1;
	}

	void RenderManager::UploadPerInstanceData(
		const void* pData, 
		const UINT byteWidth,
		const UINT offset)
	{
		switch (m_firstTimeUpload)
		{
		case TRUE:
			graphics::UploadToDynamicBuffer(
				m_pContext4,
				m_pPerObjectBuffer,
				D3D11_MAP_WRITE_DISCARD,
				pData,
				byteWidth,
				offset);
			m_firstTimeUpload = FALSE;
			break;

		default:
			graphics::UploadToDynamicBuffer(
				m_pContext4,
				m_pPerObjectBuffer,
				D3D11_MAP_WRITE_NO_OVERWRITE,
				pData,
				byteWidth,
				offset);
			break;
		}
	}

	void RenderManager::SetShaderModelLayout(const UINT shader, const ShaderModelLayout& rLayout)
	{
		m_shaders[shader].Layout = rLayout;
	}

	void RenderManager::BeginUpload()
	{
		m_firstTimeUpload = TRUE;
	}

	void RenderManager::UpdatePipeline(const UINT pipeline, const void* pPipelineData)
	{
		m_pipelines[pipeline]->Update(m_pContext4, pPipelineData);
	}

	void RenderManager::ExecutePipeline(const UINT pipeline)
	{
		this->ExecutePipeline(pipeline, 0, (UINT)m_shaders.size());
	}

	void RenderManager::ExecutePipeline(const UINT pipeline, const UINT shader)
	{
		this->ExecutePipeline(pipeline, shader, shader);
	}

	void RenderManager::ExecutePipeline(
		const UINT pipeline, 
		const UINT programStartIndex, 
		const UINT programEndIndex)
	{
		GraphicsPipeline* pPipeline = m_pipelines[pipeline];

		pPipeline->Begin(m_pContext4);

		UINT data_location = 0;
		const UINT shader_count = (UINT)m_shaders.size();

		const UINT start	= max(programStartIndex, 0);
		const UINT end		= min(programEndIndex + 1, shader_count);

		for (UINT i = start; i < end; i++)
		{
			ShaderModelLayout& layout	= m_shaders[i].Layout;
			RenderProgram& program		= m_shaders[i].Program;

			UINT total_models = 0;
			for (UINT i = 0; i < layout.MeshCount; i++)
			{
				total_models += layout.pInstanceCountPerMesh[i];
			}

			if (total_models > 0)
			{
				pPipeline->PreProcess(
					m_pContext4,
					program.pVertexShader,
					program.pPixelShader);

				switch(program.PerObjectByteWidth)
				{
				case 0:
					graphics::DrawMeshes(
						m_pContext4,
						layout.MeshCount,
						layout.pMeshes,
						layout.pInstanceCountPerMesh);
					break;

				default:
					graphics::DrawMeshes(
						m_pContext4,
						m_pPerObjectBuffer,
						data_location,
						program.PerObjectByteWidth,
						layout.MeshCount,
						layout.pMeshes,
						layout.pInstanceCountPerMesh);

					data_location += total_models * program.PerObjectByteWidth;
					data_location = (UINT)(ceil(data_location / 256.f) * 256);
					break;
				}
			}
		}

		pPipeline->End(m_pContext4);
	}
}