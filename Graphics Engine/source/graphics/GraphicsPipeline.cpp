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

		UINT compile_flag = 0;

#ifdef _DEBUG
		compile_flag = D3DCOMPILE_DEBUG;

#else // _RELEASE
		compile_flag = D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

		D3DCompile(
			pShaderCode,
			length,
			NULL,
			NULL,
			NULL, 
			"main",
			pTarget,
			compile_flag,
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

	int GraphicsPipeline::CreateInputLayout(
		ID3D11Device4* pDevice4,
		ID3DBlob* pVertexShader)
	{
		D3D11_INPUT_ELEMENT_DESC desc[6];

		desc[0].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		desc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		desc[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		desc[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		desc[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		desc[5].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;

		// ---

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

		// ---

		desc[3].Format = DXGI_FORMAT_R16_UINT;
		desc[3].InputSlot = 3;
		desc[3].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
		desc[3].InstanceDataStepRate = 1;
		desc[3].SemanticIndex = 0;
		desc[3].SemanticName = "INDEX";

		// ---

		desc[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		desc[4].InputSlot = 4;
		desc[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		desc[4].InstanceDataStepRate = 0;
		desc[4].SemanticIndex = 0;
		desc[4].SemanticName = "BLENDWEIGHT";

		desc[5].Format = DXGI_FORMAT_R32G32B32A32_SINT;
		desc[5].InputSlot = 5;
		desc[5].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
		desc[5].InstanceDataStepRate = 0;
		desc[5].SemanticIndex = 0;
		desc[5].SemanticName = "BLENDINDICES";

		HRESULT hr = pDevice4->CreateInputLayout(
			desc,
			ARRAYSIZE(desc),
			pVertexShader->GetBufferPointer(),
			pVertexShader->GetBufferSize(),
			&m_pLayout);

		if (FAILED(hr))	return FALSE;

		return TRUE;
	}

	bool GraphicsPipeline::CreateVertexShader(
		ID3D11Device4* pDevice4,
		const std::string& shaderCode)
	{
		int result;
		HRESULT hr;

		// Compile Shader
		ID3DBlob* pShader = NULL;
		result = CompileShader(
			shaderCode.c_str(),
			(UINT)shaderCode.length(),
			"vs_5_0",
			&pShader);

		if (!result) return FALSE;
		
		// Create Vertex Shader With Compiled Code
		hr = pDevice4->CreateVertexShader(
			pShader->GetBufferPointer(),
			pShader->GetBufferSize(),
			NULL,
			&m_pVertexShader);

		if (FAILED(hr))
		{
			pShader->Release();
			return FALSE;
		}

		// Create Input Layout
		result = CreateInputLayout(pDevice4, pShader);
		if (!result) return FALSE;

		return TRUE;
	}

	bool GraphicsPipeline::CreatePixelShader(
		ID3D11Device4* pDevice4,
		const std::string& shaderCode)
	{
		int result;
		HRESULT hr;

		// Compile Shader
		ID3DBlob* pShader = NULL;
		result = CompileShader(
			shaderCode.c_str(),
			(UINT)shaderCode.length(),
			"ps_5_0",
			&pShader);

		if (!result) return FALSE;

		// Create Pixel Shader With Compiled Code
		hr = pDevice4->CreatePixelShader(
			pShader->GetBufferPointer(),
			pShader->GetBufferSize(),
			NULL,
			&m_pPixelShader);

		if (FAILED(hr)) return FALSE;

		return TRUE;
	}

	GraphicsPipeline::GraphicsPipeline()
	{
		m_pLayout		= NULL;
		m_pVertexShader = NULL;
		m_pPixelShader	= NULL;
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

		result = CreateVertexShader(pDevice4, vertexShader);
		if (!result) return FALSE;

		result = CreatePixelShader(pDevice4, pixelShader);
		if (!result) return FALSE;

		return TRUE;
	}

	void GraphicsPipeline::Release()
	{
		m_pLayout->Release();
		m_pVertexShader->Release();
		m_pPixelShader->Release();
	}
}
