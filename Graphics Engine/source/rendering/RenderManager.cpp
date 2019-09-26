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

		char* pByte = (char*)malloc(totalByteWidth);

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

		m_meshManager.m_pDevice			= m_pDevice;
		m_drawManager.m_pMeshManager	= &m_meshManager;

		m_techniques.InitializeAll(m_pDevice);
		m_pContext->UploadBufferToGPU(graphics::BUFFER_UPLOAD_STATIC_DATA);

		m_techniques.SetDrawManager(&m_drawManager);
	}

	void RenderManager::Submit(const Model& rModel)
	{
	}

	void RenderManager::Draw()
	{
		m_techniques.UpdateAll(m_pContext);

		m_pContext->UploadBufferToGPU(graphics::BUFFER_UPLOAD_DYNAMIC_DATA);

		m_techniques.SetAndDrawAll(m_pContext);
	}

	void RenderManager::Destroy()
	{
		m_techniques.DeconstructAll(m_pDevice);
		graphics::DeleteDeviceInterface(m_pDevice);
	}

	MeshManager* RenderManager::pGetMeshManager()
	{
		return &m_meshManager;
	}
}