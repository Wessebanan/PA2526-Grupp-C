#include "../../includes/rendering/RenderManager.h"

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

	void RenderManager::Initialize()
	{
		graphics::CreateDeviceInterface(&m_pDevice);
		m_pContext = m_pDevice->GetRenderContext();

		m_drawManager.m_pMeshManager	= &m_meshManager;
		m_drawManager.m_pBuffer			= &m_meshDataRegion;

		m_techniques.InitializeAll(m_pDevice);
		m_pContext->UploadBufferToGPU(graphics::BUFFER_UPLOAD_STATIC_DATA);

		m_techniques.SetDrawManager(&m_drawManager);
	}

	void RenderManager::Draw()
	{
		m_drawManager.Clear();

		m_pContext->CopyDataToRegion(
			m_meshManager.m_pModelData,
			m_meshManager.m_modelDataSize,
			m_meshDataRegion);

		m_techniques.UpdateAll(m_pContext);

		m_pContext->UploadBufferToGPU(graphics::BUFFER_UPLOAD_DYNAMIC_DATA);

		m_techniques.SetAndDrawAll(m_pContext);
	}

	void RenderManager::Destroy()
	{
		m_techniques.DeconstructAll(m_pDevice);

		free(m_pMemoryForTechniques);
		graphics::DeleteDeviceInterface(m_pDevice);
	}

	int RenderManager::LoadMeshes(const LOAD_MESH_DESC* pDesc, const UINT count)
	{
		if (!m_meshManager.LoadMeshes(m_pDevice, pDesc, count)) return FALSE;

		m_pContext->UploadBufferToGPU(graphics::BUFFER_UPLOAD_VERTEX_DATA);
		m_pContext->UploadBufferToGPU(graphics::BUFFER_UPLOAD_INDEX_DATA);

		return TRUE;
	}
}