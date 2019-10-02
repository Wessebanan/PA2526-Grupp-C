#include "../../includes/rendering/RenderManager.h"
#include <iostream>

namespace rendering
{

	RenderManager::RenderManager()
	{
		m_pDevice	= NULL;
		m_pContext	= NULL;


		RENDER_TECHNIQUE_DESC arr[RENDER_TECHNIQUES_COUNT];
		m_techniques.ConstructAll(arr);

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
		const UINT clientWidth,
		const UINT clientHeight,
		const char* pTitle)
	{
		graphics::CreateDeviceInterface(&m_pDevice);
		m_pContext = m_pDevice->GetRenderContext();

		m_pDevice->CreateDynamicBufferRegion(
			sizeof(DirectX::XMFLOAT4X4),
			NULL,
			&m_viewMatrixRegion);

		DirectX::XMFLOAT4X4 projection;
		XMStoreFloat4x4(&projection,
			DirectX::XMMatrixPerspectiveFovLH(
				3.14f / 2.0f, 
				clientWidth / (float)clientHeight, 
				0.1f, 
				100.0f));

		m_pDevice->CreateStaticBufferRegion(
			sizeof(DirectX::XMFLOAT4X4),
			&projection,
			&m_projectionMatrixRegion);

		m_pContext->VSSetConstantBuffer(1, m_viewMatrixRegion);
		m_pContext->VSSetConstantBuffer(2, m_projectionMatrixRegion);

		m_pContext->SetViewport(
			0, 
			0, 
			clientWidth, 
			clientHeight);

		m_pDevice->CreatePresentWindow(
			clientWidth,
			clientHeight,
			pTitle,
			&m_target,
			&m_wnd);

		m_pDevice->CreateDepthBuffer(
			clientWidth, 
			clientHeight, 
			&m_depthBuffer);

		m_drawManager.m_pMeshManager	= &m_meshManager;

		m_techniques.InitializeAll(m_pDevice);
		m_techniques.SetDrawManager(&m_drawManager);

		m_pContext->UploadBufferToGPU(graphics::BUFFER_UPLOAD_STATIC_DATA);
	}

	void RenderManager::Clear()
	{
		m_pContext->ClearDepth(m_depthBuffer);
		m_pContext->ClearRenderTarget(m_target, 0.0f, 0.0f, 0.0f);
	}

	void RenderManager::Draw()
	{
		m_pContext->SetRenderTarget(m_target, m_depthBuffer);

		m_pContext->CopyDataToRegion(
			m_meshManager.m_pPerInstanceData,
			m_meshManager.m_perInstanceDataSize,
			m_modelDataRegion);

		m_techniques.UpdateAll(m_pContext);

		m_pContext->UploadBufferToGPU(graphics::BUFFER_UPLOAD_DYNAMIC_DATA);

		m_techniques.SetAndDrawAll(m_pContext);
	}

	void RenderManager::Destroy()
	{
		m_techniques.DeconstructAll(m_pDevice);

		m_pDevice->DeleteRenderTarget(m_target);
		m_pDevice->DeleteDepthBuffer(m_depthBuffer);
		m_pDevice->DeletePresentWindow(m_wnd);

		free(m_pMemoryForTechniques);
		graphics::DeleteDeviceInterface(m_pDevice);
	}

	int RenderManager::CreateMesh(
		const VERTEX_BUFFER_DATA* pVertexData, 
		const INDEX_BUFFER_DATA* pIndexData)
	{
		return m_meshManager.CreateMesh(pVertexData, pIndexData, m_pDevice);
	}

	void RenderManager::CreateModelHeap(
		const TECHNIQUE_HEAP_LAYOUT_DESC layoutDesc[RENDER_TECHNIQUES_COUNT])
	{
		// Since every mesh should be created at this moment upload all data
		m_pContext->UploadBufferToGPU(graphics::BUFFER_UPLOAD_VERTEX_DATA);
		m_pContext->UploadBufferToGPU(graphics::BUFFER_UPLOAD_INDEX_DATA);

		// Create Model Heap
		m_meshManager.CreateModelHeap(layoutDesc);

		// Create Region on GPU for the model data
		m_pDevice->CreateDynamicBufferRegion(
			m_meshManager.m_perInstanceDataSize, 
			NULL, 
			&m_modelDataRegion);
		m_drawManager.m_pBuffer = &m_modelDataRegion;
	}

	void* RenderManager::GetTechniqueModelBuffer(
		const RENDER_TECHNIQUES techniqueIndex)
	{
		return m_meshManager.GetTechniqueModelBuffer(techniqueIndex);
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
}