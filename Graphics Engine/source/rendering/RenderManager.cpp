#include "../../includes/rendering/RenderManager.h"
#include <iostream>

namespace rendering
{

	RenderManager::RenderManager()
	{
		m_pContext	= NULL;

		m_clientWidth = 0;
		m_clientHeight = 0;

		RENDER_TECHNIQUE_DESC arr[RENDER_TECHNIQUES_COUNT];
		m_geometry.ConstructAll(arr);

		size_t totalByteWidth = 0;
		for (unsigned int i = 0; i < RENDER_TECHNIQUES_COUNT; i++)
		{
			totalByteWidth += arr[i].ByteWidth;
		}

		m_pMemoryForTechniques = (char*)malloc(totalByteWidth);
		char* pByte = m_pMemoryForTechniques;

		for (unsigned int i = 0; i < RENDER_TECHNIQUES_COUNT; i++)
		{
			(*arr[i].ppData) = pByte;
			pByte += arr[i].ByteWidth;
		}
	}

	RenderManager::~RenderManager()
	{
	}

	void RenderManager::Initialize(
		const SUN_DESC& sunDesc,
		const UINT clientWidth,
		const UINT clientHeight,
		const char* pTitle)
	{
		m_device.Initialize();
		m_pContext = m_device.GetRenderContext();

		m_clientWidth  = clientWidth;
		m_clientHeight = clientHeight;

		// --- Main Camera ---

		m_device.CreateDynamicBufferRegion(
			sizeof(DirectX::XMFLOAT4X4),
			NULL,
			&m_viewMatrixRegion);

		DirectX::XMFLOAT4X4 projection;
		XMStoreFloat4x4(&projection,
			DirectX::XMMatrixPerspectiveFovLH(
				3.14f / 2.0f, 
				m_clientWidth / (float)m_clientHeight,
				0.1f, 
				100.0f));

		m_device.CreateStaticBufferRegion(
			sizeof(DirectX::XMFLOAT4X4),
			&projection,
			&m_projectionMatrixRegion);

		m_device.CreateDepthBuffer(
			m_clientWidth,
			m_clientHeight,
			&m_depthBuffer,
			false);

		// --- Window (With Back Buffer) ---

		m_device.CreatePresentWindow(
			m_clientWidth,
			m_clientHeight,
			pTitle,
			&m_target,
			&m_wnd);

		// --- Sun / Shading Data (Directional Light Shading) ---


		DirectX::XMVECTOR sunDir = DirectX::XMVectorSet(
			sunDesc.Direction.x * -1.0f, 
			sunDesc.Direction.y * -1.0f, 
			sunDesc.Direction.z * -1.0f,
			0.0f
			);

		sunDir = DirectX::XMVector3Normalize(sunDir);

		DirectX::XMStoreFloat3(&m_sunData.Direction, sunDir);
		m_sunData.packedData = PACK(
			sunDesc.Red, 
			sunDesc.Green, 
			sunDesc.Blue, 
			255);

		m_device.CreateStaticBufferRegion(
			sizeof(DirectionalLight),
			&m_sunData,
			&m_sunDataRegion);

		// --- Sun / Shadow Map Camera ---

		DirectX::XMFLOAT4X4 sun_view_matrix;
		DirectX::XMStoreFloat4x4(&sun_view_matrix,
			DirectX::XMMatrixLookToLH(
				{ sunDesc.Position.x, sunDesc.Position.y, sunDesc.Position.z },
				{ sunDesc.Direction.x, sunDesc.Direction.y, sunDesc.Direction.z },
				{ 0.0f, 1.0f,  0.0f }
		));

		DirectX::XMFLOAT4X4 sun_proj_matrix;
		XMStoreFloat4x4(&sun_proj_matrix,
			DirectX::XMMatrixOrthographicLH(
				sunDesc.Width, sunDesc.Height, sunDesc.NearPlane, sunDesc.FarPlane));

		m_device.CreateStaticBufferRegion(
			sizeof(DirectX::XMFLOAT4X4),
			&sun_view_matrix,
			&m_sunViewMatrix);

		m_device.CreateStaticBufferRegion(
			sizeof(DirectX::XMFLOAT4X4),
			&sun_proj_matrix,
			&m_sunProjMatrix);

		m_device.CreateDepthBuffer(
			sunDesc.Resolution,
			sunDesc.Resolution,
			&m_shadowMap,
			true);

		// --- END ---

		m_geometry.InitializeAll(&m_device);
		m_geometry.SetDrawManager(&m_drawManager);

		m_pContext->UploadBufferToGPU(graphics::BUFFER_UPLOAD_STATIC_DATA);

		// Static Binds
		m_pContext->VSSetConstantBuffer(5, m_sunDataRegion);
	}

	void RenderManager::Clear(const float red, const float green, const float blue)
	{
		m_pContext->ClearDepth(m_depthBuffer);
		m_pContext->ClearDepth(m_shadowMap);
		m_pContext->ClearRenderTarget(m_target, red, green, blue);
	}

	void RenderManager::Draw()
	{
		// Upload Necessary Data To Render Geometry
		m_drawManager.Begin(m_pContext);
		m_geometry.UpdateAll(m_pContext);
		m_pContext->UploadBufferToGPU(graphics::BUFFER_UPLOAD_DYNAMIC_DATA);

		DrawShadowMap();

		DrawGeometry();
	}

	void RenderManager::Destroy()
	{
		m_geometry.DeconstructAll(&m_device);

		m_device.DeleteRenderTarget(m_target);
		m_device.DeleteDepthBuffer(m_depthBuffer);
		m_device.DeletePresentWindow(m_wnd);

		m_device.DeleteDepthBuffer(m_shadowMap);

		free(m_pMemoryForTechniques); 
		m_device.Release();
	}

	int RenderManager::CreateMesh(
		const VERTEX_BUFFER_DATA* pVertexData, 
		const INDEX_BUFFER_DATA* pIndexData)
	{
		return m_drawManager.CreateMesh(pVertexData, pIndexData, &m_device);
	}

	void RenderManager::CreateModelHeap(
		const TECHNIQUE_HEAP_LAYOUT_DESC layoutDesc[RENDER_TECHNIQUES_COUNT])
	{
		// Since every mesh should be created at this moment upload all data
		m_pContext->UploadBufferToGPU(graphics::BUFFER_UPLOAD_VERTEX_DATA);
		m_pContext->UploadBufferToGPU(graphics::BUFFER_UPLOAD_INDEX_DATA);

		// Create Model Heap
		m_drawManager.CreateModelHeap(layoutDesc, &m_device);
	}

	void* RenderManager::GetTechniqueModelBuffer(
		const RENDER_TECHNIQUES techniqueIndex)
	{
		return m_drawManager.GetTechniqueModelBuffer(techniqueIndex);
	}

	void RenderManager::SetViewMatrix(const DirectX::XMFLOAT4X4 viewMatrix)
	{
		m_pContext->CopyDataToRegion(
			&viewMatrix,
			sizeof(DirectX::XMFLOAT4X4),
			m_viewMatrixRegion);
	}

	graphics::PresentWindow* RenderManager::GetPresentWindow()
	{
		return &m_wnd;
	}

	// Render Shadow Map
	void RenderManager::DrawShadowMap()
	{
		// Where To Draw
		m_pContext->SetRenderTarget(NULL, &m_shadowMap);

		// Set Camera Data
		m_pContext->VSSetConstantBuffer(1, m_sunViewMatrix);
		m_pContext->VSSetConstantBuffer(2, m_sunProjMatrix);

		// Set Size To Draw
		m_pContext->SetViewport(
			0,
			0,
			1024,
			1024);

		// Draw All
		m_geometry.DrawAllWithoutPS(m_pContext);
	}

	// Render Geometry
	void RenderManager::DrawGeometry()
	{
		ID3D11DeviceContext4* pInternal = m_pContext->GetInternalContext();

		// Where To Draw
		m_pContext->SetRenderTarget(&m_target, &m_depthBuffer);

		graphics::BufferRegion regions[4];
		// Set Camera Data
		regions[0] = m_viewMatrixRegion;
		regions[1] = m_projectionMatrixRegion;

		// Set Light Data
		regions[2] = m_sunViewMatrix;
		regions[3] = m_sunProjMatrix;

		m_pContext->VSSetConstantBuffers<4>(1, regions);

		// Set Shadow Map
		pInternal->PSSetShaderResources(0, 1, &m_shadowMap.pResource);

		// Set Size To Draw
		m_pContext->SetViewport(
			0,
			0,
			m_clientWidth,
			m_clientHeight);

		// Draw All
		m_geometry.DrawAll(m_pContext);

		ID3D11ShaderResourceView* pView = NULL;
		pInternal->PSSetShaderResources(0, 1, &pView);
	}
}