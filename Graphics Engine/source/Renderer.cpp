#include "../includes/pch.h"
#include "../includes/Renderer.h"

const std::string gVertexShader = R"(

cbuffer gTransformation : register (b0)
{
	float4x4 gWorld[3];
};

cbuffer gCam : register (b1)
{
	float4x4 gView;
};

cbuffer gPer : register (b2)
{
	float4x4 gPerspective;
};

struct VSOUT
{
	float4 pos	: SV_POSITION;
	float2 uv	: UV0;
};

VSOUT main(
	float3 pos : POSITION0, 
	float2 uv : UV0,
	uint instance : INDEX0)
{
	VSOUT output;

	float4x4 wvp = mul(gPerspective, mul(gView, gWorld[instance]));

	output.pos	= mul(wvp, float4(pos, 1.0f));
	output.uv	= uv;
	
	return output;
}	

)";

const std::string gPixelShader = R"(

struct PSIN
{
	float4 pos	: SV_POSITION;
	float2 uv	: UV0;
};

float4 main(PSIN input) : SV_TARGET
{ 
	return float4(input.uv.x, 1.0f, input.uv.y, 1.0f);
}	

)";

namespace graphics
{
	Renderer::Renderer()
	{
		m_pDevice = NULL;
		m_meshCount = 0;
		m_drawCount = 0;
	}

	Renderer::~Renderer()
	{
	}

	void Renderer::Initialize(
		const UINT width, 
		const UINT height, 
		const char* pTitle)
	{
		CreateDeviceInterface(&m_pDevice);
		m_pContext = m_pDevice->GetRenderContext();

		m_pDevice->CreatePresentWindow(
			width,
			height,
			pTitle,
			&m_backBuffer,
			&m_window);

		m_pDevice->CreateDepthBuffer(
			width,
			height,
			&m_depthBuffer);

		m_pContext->SetViewport(0, 0, width, height);

		XMStoreFloat4x4(&m_view,
			XMMatrixLookToLH(
				{ 5.5f, 0.0f, -20.0f },
				{ 1.0f, 1.0f, 1.0f },
				{ 0.0f, 1.0f, 0.0f }));

		XMStoreFloat4x4(&m_projection,
			XMMatrixPerspectiveFovLH(
				3.14f / 2.0f, 
				width / (float)height, 
				0.1f, 
				800.0f));

		m_pDevice->CreateDynamicBufferRegion(
			sizeof(m_view),
			NULL,
			&m_viewRegion);

		m_pDevice->CreateDynamicBufferRegion(
			sizeof(XMFLOAT4X4) * MAXIMUM_DRAW_LIST_SIZE,
			NULL,
			&m_buffer0);

		m_pDevice->CreateStaticBufferRegion(
			sizeof(m_projection),
			&m_projection,
			&m_projRegion);

		m_pContext->VSSetConstantBuffer(1, m_viewRegion);
		m_pContext->VSSetConstantBuffer(2, m_projRegion);
	}

	int Renderer::CreateMesh(
		const VERTEX_BUFFER_DATA* pVertexData,
		const INDEX_BUFFER_DATA* pIndexData,
		GPUMesh& rMesh)
	{
		// If there's no data or maximum allowed is reached
		if (!pVertexData || m_meshCount >= MAXIMUM_MESHES) return FALSE;

		const int buffer_index = m_meshCount;

		// Create vertex buffer
		int result = m_pDevice->CreateVertexBufferRegion(
			pVertexData->VertexCount,
			pVertexData->pVertexData,
			pVertexData->pNormalsData,
			pVertexData->pTextureCoordData,
			&m_vertexBuffers[buffer_index]);

		if (!result) return FALSE;

		m_meshCount++;
		rMesh.BufferIndex	= buffer_index;
		rMesh.IsIndexed		= FALSE;

		// if there's index data, create a region for that too
		if (pIndexData)
		{
			m_pDevice->CreateIndexBufferRegion(
				pIndexData->IndexCount,
				pIndexData->pIndexData,
				&m_indexBuffers[rMesh.BufferIndex]);

			rMesh.IsIndexed = TRUE;
		}

		return TRUE;
	}

	GPUShader Renderer::CreateShader()
	{
		m_pDevice->CreateGraphicsPipeline(
			gVertexShader,
			gPixelShader,
			&m_pPipelines[0]);

		GPUShader shader;
		shader.Index = 0;

		return shader;
	}

	void Renderer::Clear()
	{
		m_drawCount = 0;

		m_pContext->ClearDepth(m_depthBuffer);
		m_pContext->ClearRenderTarget(m_backBuffer, 0.0f, 0.0f, 0.0f);
	}

	void Renderer::Submit(const Model model)
	{
		m_drawList[m_drawCount] = model;
		m_drawCount++;
	}

	void Renderer::Draw()
	{
	}

	void Renderer::Release()
	{
		m_pDevice->DeleteRenderTarget(m_backBuffer);
		m_pDevice->DeleteDepthBuffer(m_depthBuffer);

		m_pDevice->DeletePresentWindow(m_window);

		DeleteDeviceInterface(m_pDevice);
	}

	PresentWindow* Renderer::GetWindow()
	{
		return &m_window;
	}
}