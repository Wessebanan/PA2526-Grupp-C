#include "../includes/GraphicsPipeline.h"

namespace graphics
{

	void CompileShader(
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
		}
	}

	bool CreateInputLayout(
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

		pDevice4->CreateInputLayout(
			desc,
			ARRAYSIZE(desc),
			pVertexShader->GetBufferPointer(),
			pVertexShader->GetBufferSize(),
			ppLayout);

		return true;
	}

	bool CreateVertexShader(
		ID3D11Device4* pDevice4,
		const std::string& shaderCode,
		ID3D11VertexShader** ppVertexShader,
		ID3D11InputLayout** ppInputLayout)
	{
		ID3DBlob* pShader = NULL;
		CompileShader(
			shaderCode.c_str(),
			(UINT)shaderCode.length(),
			"vs_5_0",
			&pShader);

		pDevice4->CreateVertexShader(
			pShader->GetBufferPointer(),
			pShader->GetBufferSize(),
			NULL,
			ppVertexShader);

		CreateInputLayout(
			pDevice4,
			pShader,
			ppInputLayout);

		return true;
	}

	bool CreatePixelShader(
		ID3D11Device4* pDevice4,
		const std::string& shaderCode,
		ID3D11PixelShader** ppPixelShader)
	{
		ID3DBlob* pShader = NULL;
		CompileShader(
			shaderCode.c_str(),
			(UINT)shaderCode.length(),
			"ps_5_0",
			&pShader);

		pDevice4->CreatePixelShader(
			pShader->GetBufferPointer(),
			pShader->GetBufferSize(),
			NULL,
			ppPixelShader);

		return true;
	}

	GraphicsPipeline::GraphicsPipeline()
	{
	}

	GraphicsPipeline::~GraphicsPipeline()
	{
	}

	void GraphicsPipeline::Initialize(
		ID3D11Device4* pDevice4,
		const std::string& vertexShader,
		const std::string& pixelShader)
	{
		CreateVertexShader(
			pDevice4,
			vertexShader,
			&m_pVertexShader,
			&m_pLayout);

		CreatePixelShader(
			pDevice4,
			pixelShader,
			&m_pPixelShader);
	}

	void GraphicsPipeline::Release()
	{
		m_pLayout->Release();
		m_pVertexShader->Release();
		m_pPixelShader->Release();
	}
}
