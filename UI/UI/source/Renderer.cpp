//#include "Renderer.h"
//
//const std::string gVertexShader = R"(
//
//cbuffer gTransformation : register (b0)
//{
//	float4x4 gWorld[3];
//};
//
//cbuffer gCam : register (b1)
//{
//	float4x4 gView;
//};
//
//cbuffer gPer : register (b2)
//{
//	float4x4 gPerspective;
//};
//
//struct VSOUT
//{
//	float4 pos	: SV_POSITION;
//	float2 uv	: UV0;
//};
//
//VSOUT main(
//	float3 pos : POSITION0, 
//	float2 uv : UV0,
//	uint instance : INDEX0)
//{
//	VSOUT output;
//
//	float4x4 wvp = mul(gPerspective, mul(gView, gWorld[instance]));
//
//	output.pos	= mul(wvp, float4(pos, 1.0f));
//	output.uv	= uv;
//	
//	return output;
//}	
//
//)";
//
//const std::string gPixelShader = R"(
//
//struct PSIN
//{
//	float4 pos	: SV_POSITION;
//	float2 uv	: UV0;
//};
//
//float4 main(PSIN input) : SV_TARGET
//{ 
//	return float4(input.uv.x, 1.0f, input.uv.y, 1.0f);
//}	
//
//)";
//
//namespace graphics
//{
//	Renderer::Renderer()
//	{
//		m_pDevice = NULL;
//	}
//
//	Renderer::~Renderer()
//	{
//	}
//
//	void Renderer::Initialize(
//		const UINT width, 
//		const UINT height, 
//		const char* pTitle)
//	{
//		CreateDeviceInterface(&m_pDevice);
//		m_pContext = m_pDevice->GetRenderContext();
//
//		m_pDevice->CreatePresentWindow(
//			width,
//			height,
//			"D3D11",
//			&m_backBuffer,
//			&m_pWindow);
//
//		m_pDevice->CreateDepthBuffer(
//			width,
//			height,
//			&m_depthBuffer);
//
//		m_pDevice->CreatePipeline(
//			gVertexShader,
//			gPixelShader,
//			&m_pPipeline);
//
//		m_pContext->SetViewport(0, 0, width, height);
//		m_pContext->SetGraphicsPipeline(m_pPipeline);
//
//		XMStoreFloat4x4(&m_view,
//			XMMatrixLookToLH(
//				{ 5.5f, 0.0f, -20.0f },
//				{ 1.0f, 1.0f, 1.0f },
//				{ 0.0f, 1.0f, 0.0f }));
//
//		XMStoreFloat4x4(&m_projection,
//			XMMatrixPerspectiveFovLH(
//				3.14f / 2.0f, 
//				width / (float)height, 
//				0.1f, 
//				800.0f));
//
//		m_pDevice->CreateDynamicBufferRegion(
//			sizeof(m_view),
//			NULL,
//			&m_viewRegion);
//
//		m_pDevice->CreateDynamicBufferRegion(
//			sizeof(XMFLOAT4X4) * MAXIMUM_RENDER_OBJECT_SIZE,
//			NULL,
//			&m_buffer0);
//
//		m_pDevice->CreateStaticBufferRegion(
//			sizeof(m_projection),
//			&m_projection,
//			&m_projRegion);
//
//		m_pContext->VSSetConstantBuffer(1, m_viewRegion);
//		m_pContext->VSSetConstantBuffer(2, m_projRegion);
//
//		m_pContext->UploadStaticDataToGPU();
//	}
//
//	void Renderer::Clear()
//	{
//		m_drawSize = 0;
//	}
//
//	void Renderer::Submit()
//	{
//		m_drawSize++;
//	}
//
//	void Renderer::Draw()
//	{
//		m_pContext->ClearDepth(m_depthBuffer);
//		m_pContext->ClearRenderTarget(m_backBuffer, 0.0f, 0.0f, 0.0f);
//
//		m_pContext->SetRenderTarget(
//			m_backBuffer, 
//			m_depthBuffer);
//
//		// Copy Data to CPU Buffer (View Matrix)
//		m_pContext->CopyDataToRegion(
//			&m_view,
//			sizeof(m_view),
//			m_viewRegion);
//
//		// Copy Data to CPU Buffer (World Matrices)
//		m_pContext->CopyDataToRegion(
//			NULL,
//			m_drawSize * sizeof(XMFLOAT4X4),
//			m_buffer0);
//
//		// Upload All Data to GPU
//		m_pContext->UploadDynamicDataToGPU();
//
//		UINT at		= m_buffer0.DataLocation;
//		UINT end	= m_buffer0.DataLocation + m_buffer0.DataCount;
//
//		BufferRegion r = m_buffer0;
//		r.DataCount = RenderContext::CB_MAX_BYTES_PER_BIND;
//
//		UINT meshIndex = 0;
//		std::map<UINT, UINT> meshCount = mrData.m_meshCount;
//		while (at < end && meshIndex < 2)
//		{
//			r.DataLocation = at;							// Set active region (65536 bytes per draw)
//			m_pContext->VSSetConstantBuffer(0, r);			// Apply active Region
//
//			UINT drawn = 0;
//			while (drawn < 1024 && meshIndex < 2)
//			{
//				UINT count = meshCount[meshIndex];
//
//				if (count + drawn > 1024)
//				{
//					count = 1024 - drawn;
//				}
//
//				meshCount[meshIndex] -= count;
//
//				m_pContext->DrawInstanced(
//					count, 
//					drawn, 
//					meshes[meshIndex]);
//
//				drawn += count;
//
//				if (meshCount[meshIndex] == 0)
//				{
//					meshIndex++;
//				}
//			}
//
//			at += RenderContext::CB_MAX_BYTES_PER_BIND;									// Increment active region
//		}
//	}
//
//	void Renderer::Present()
//	{
//		m_pWindow->Present();
//	}
//
//	void Renderer::Release()
//	{
//		m_pDevice->DeleteRenderTarget(m_backBuffer);
//		m_pDevice->DeleteDepthBuffer(m_depthBuffer);
//		m_pDevice->DeletePipeline(m_pPipeline);
//
//		DeleteDeviceInterface(m_pDevice);
//	}
//}