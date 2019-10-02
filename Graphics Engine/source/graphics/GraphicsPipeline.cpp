#include "../../includes/graphics/GraphicsPipeline.h"

namespace graphics
{
	int CompileShader(
		const char* pShaderCode,
		const UINT length,
		const char* pTarget,
		ID3DBlob** ppBlob)
	{
		ID3DBlob* pErrorBlob = NULL;

		D3DCompile(
			pShaderCode,
			length,
			NULL,
			NULL,
			NULL, 
			"main",
			pTarget,
			0,
			0,
			ppBlob,
			&pErrorBlob);

		if (pErrorBlob)
		{
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
			pErrorBlob->Release();
			return FALSE;
		}

		return TRUE;
	}

	int CreateInputLayout(
		ID3D11Device4* pDevice4,
		ID3DBlob* pVertexShader,
		ID3D11InputLayout** ppLayout)
	{
		D3D11_INPUT_ELEMENT_DESC desc[4];

		desc[0].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		desc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		desc[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		desc[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;

		desc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		desc[0].InputSlot = 0;
		desc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		desc[0].InstanceDataStepRate = 0;
		desc[0].SemanticIndex = 0;
		desc[0].SemanticName = "POSITION";

		desc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		desc[1].InputSlot = 1;
		desc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		desc[1].InstanceDataStepRate = 0;
		desc[1].SemanticIndex = 0;
		desc[1].SemanticName = "NORMAL";

		desc[2].Format = DXGI_FORMAT_R32G32_FLOAT;
		desc[2].InputSlot = 2;
		desc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		desc[2].InstanceDataStepRate = 0;
		desc[2].SemanticIndex = 0;
		desc[2].SemanticName = "UV";

		desc[3].Format = DXGI_FORMAT_R16_UINT;
		desc[3].InputSlot = 3;
		desc[3].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
		desc[3].InstanceDataStepRate = 1;
		desc[3].SemanticIndex = 0;
		desc[3].SemanticName = "INDEX";

		HRESULT hr = pDevice4->CreateInputLayout(
			desc,
			ARRAYSIZE(desc),
			pVertexShader->GetBufferPointer(),
			pVertexShader->GetBufferSize(),
			ppLayout);

		if (FAILED(hr))	return FALSE;

		return TRUE;
	}

	bool CreateVertexShader(
		ID3D11Device4* pDevice4,
		const std::string& shaderCode,
		ID3D11VertexShader** ppVertexShader,
		ID3D11InputLayout** ppInputLayout)
	{
		int result;
		HRESULT hr;

		ID3DBlob* pShader = NULL;
		result = CompileShader(
			shaderCode.c_str(),
			(UINT)shaderCode.length(),
			"vs_5_0",
			&pShader);

		if (!result) return FALSE;

		hr = pDevice4->CreateVertexShader(
			pShader->GetBufferPointer(),
			pShader->GetBufferSize(),
			NULL,
			ppVertexShader);

		if (FAILED(hr))
		{
			pShader->Release();
			return FALSE;
		}

		result = CreateInputLayout(
			pDevice4,
			pShader,
			ppInputLayout);

		if (!result) return FALSE;

		return TRUE;
	}

	bool CreatePixelShader(
		ID3D11Device4* pDevice4,
		const std::string& shaderCode,
		ID3D11PixelShader** ppPixelShader)
	{
		int result;
		HRESULT hr;

		ID3DBlob* pShader = NULL;
		result = CompileShader(
			shaderCode.c_str(),
			(UINT)shaderCode.length(),
			"ps_5_0",
			&pShader);

		if (!result) return FALSE;

		hr = pDevice4->CreatePixelShader(
			pShader->GetBufferPointer(),
			pShader->GetBufferSize(),
			NULL,
			ppPixelShader);

		if (FAILED(hr)) return FALSE;

		return TRUE;
	}

	UINT GraphicsPipeline::NUM_ALLOCATED = 0;

	GraphicsPipeline::GraphicsPipeline()
	{
		m_pLayout		= NULL;
		m_pVertexShader = NULL;
		m_pPixelShader	= NULL;

		m_id = NUM_ALLOCATED++;
	}

	GraphicsPipeline::~GraphicsPipeline()
	{
	}

	int GraphicsPipeline::Initialize(
		ID3D11Device4* pDevice4,
		const std::string& vertexShader,
		const std::string& pixelShader)
	{
		int result;

		result = CreateVertexShader(
			pDevice4,
			vertexShader,
			&m_pVertexShader,
			&m_pLayout);

		if (!result) return FALSE;

		result = CreatePixelShader(
			pDevice4,
			pixelShader,
			&m_pPixelShader);

		if (!result) return FALSE;

		return TRUE;
	}

	void GraphicsPipeline::Release()
	{
		m_pLayout->Release();
		m_pVertexShader->Release();
		m_pPixelShader->Release();
	}

	GraphicsPipelineArray::GraphicsPipelineArray()
	{
		m_capacity	= 0;
		m_count		= 0;
		m_start		= 0;

		m_pPipelines = NULL;
		m_pIsAvailableArray = NULL;
		m_pData = NULL;
	}

	GraphicsPipelineArray::~GraphicsPipelineArray()
	{
		free(m_pPipelines);
	}

	int GraphicsPipelineArray::Initialize(
		ID3D11Device4* pDevice4, 
		const UINT capacity)
	{
		m_capacity = capacity;

		// Allocate Space
		{
			const size_t allocationSize =
				sizeof(UINT) +
				sizeof(GraphicsPipeline);

			// Allocate a chunk for everyone to use
			m_pData = (char*)malloc(allocationSize * m_capacity);

			// Set pipelines as first in array
			m_pPipelines = (GraphicsPipeline*)m_pData;

			// set available as second in array
			UINT offset = sizeof(GraphicsPipeline) * m_capacity;
			m_pIsAvailableArray = (UINT*)(m_pData + offset);
		}

		for (UINT i = 0; i < m_capacity; i++)
		{
			m_pIsAvailableArray[i] = TRUE;
			m_pPipelines[i] = GraphicsPipeline();
		}

		m_start = m_pPipelines[0].m_id;

		return TRUE;
	}

	void GraphicsPipelineArray::Release()
	{
	}

	int GraphicsPipelineArray::CreateGraphicsPipeline(
		ID3D11Device4* pDevice4, 
		const std::string& vertexShader, 
		const std::string& pixelShader,
		GraphicsPipeline** ppPipeline)
	{
		if (m_count >= m_capacity) return FALSE;

		// Find the first available one (can be improved if needed)
		UINT i = 0;
		while (!m_pIsAvailableArray[i]) { i++; }

		int result = 
			m_pPipelines[i].Initialize(pDevice4, vertexShader, pixelShader);

		if (!result) return FALSE;

		(*ppPipeline) = &m_pPipelines[i];

		m_pIsAvailableArray[i] = FALSE;
		m_count++;

		return TRUE;
	}

	bool GraphicsPipelineArray::DeleteGraphicsPipeline(GraphicsPipeline* pPipeline)
	{
		const UINT index = pPipeline->m_id - m_start;

		m_pIsAvailableArray[index] = TRUE;
		m_count--;

		pPipeline->Release();

		return true;
	}
}
