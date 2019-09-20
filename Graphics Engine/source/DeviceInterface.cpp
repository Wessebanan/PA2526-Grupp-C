#include "../includes/DeviceInterface.h"

namespace graphics
{
	DeviceInterface::DeviceInterface()
	{
		m_pAdapter4		= NULL;
		m_pDevice4		= NULL;
		m_pFactory6		= NULL;
	}

	DeviceInterface::~DeviceInterface()
	{
	}

	void DeviceInterface::Initialize()
	{
		{
			UINT factoryFlag	= 0;
			UINT deviceFlag		= 0;

#ifdef _DEBUG
			deviceFlag	|= D3D11_CREATE_DEVICE_DEBUG;
			factoryFlag |= DXGI_CREATE_FACTORY_DEBUG;
#endif // _DEBUG

			CreateDXGIFactory2(
				factoryFlag,
				IID_PPV_ARGS(&m_pFactory6));

			m_pFactory6->EnumAdapterByGpuPreference(
				0,
				DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
				IID_PPV_ARGS(&m_pAdapter4));

			ID3D11Device* pDeviceTemp = NULL;
			ID3D11DeviceContext* pContextTemp = NULL;

			D3D11CreateDevice(
				m_pAdapter4,
				D3D_DRIVER_TYPE_UNKNOWN,
				NULL,
				deviceFlag,
				NULL,
				0,
				D3D11_SDK_VERSION,
				&pDeviceTemp,
				NULL,
				&pContextTemp);

			pDeviceTemp->QueryInterface(IID_PPV_ARGS(&m_pDevice4));
			pContextTemp->QueryInterface(IID_PPV_ARGS(&m_context.m_pContext4));

			pDeviceTemp->Release();
			pContextTemp->Release();
		}

		m_storage.Initialize(m_pDevice4);
		m_context.Initialize(m_pDevice4, &m_storage);
		m_pipelineArray.Initialize(m_pDevice4, 10);

		ResourceView view;
	}

	void DeviceInterface::CreateBufferRegion(
		const BUFFER_TYPE type, 
		const UINT size, 
		BufferRegion* pRegion)
	{
		BufferHeap* pHeap = m_storage.GetBufferHeapCPU(type);
		pHeap->AllocateRegion(size, pRegion);
	}

	void DeviceInterface::Release()
	{
		m_pAdapter4->Release();
		m_pFactory6->Release();

		if (m_window.m_pSwapChain4)
		{
			m_window.m_pSwapChain4->Release();
		}

		m_context.Release();
		m_storage.Release();

		m_pDevice4->Release();
	}

	RenderContext* DeviceInterface::GetRenderContext()
	{
		return &m_context;
	}

	UINT64 DeviceInterface::QueryVideoMemoryUsage()
	{
		DXGI_QUERY_VIDEO_MEMORY_INFO info;

		m_pAdapter4->QueryVideoMemoryInfo(
			0,
			DXGI_MEMORY_SEGMENT_GROUP_LOCAL,
			&info);

		return info.CurrentUsage;
	}

	void DeviceInterface::CreatePresentWindow(
		const UINT width, 
		const UINT height, 
		const char* pTitle,
		RenderTarget* pRenderTarget,
		PresentWindow** ppWindow)
	{
		{
			m_window.Initialize(
				m_pDevice4,
				m_pFactory6,
				width,
				height,
				pTitle);

			ID3D11Texture2D* pTexture2D = NULL;
			m_window.m_pSwapChain4->GetBuffer(
				0,
				IID_PPV_ARGS(&pTexture2D));

			if (pTexture2D)
			{
				pRenderTarget->Region.Type = BUFFER_TYPE_UNKNOWN;

				D3D11_RENDER_TARGET_VIEW_DESC desc = {};
				desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
				desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
				desc.Texture2D.MipSlice = 0;

				m_pDevice4->CreateRenderTargetView(
					pTexture2D,
					&desc,
					&pRenderTarget->pView);

				pTexture2D->Release();
			}
		}

		(*ppWindow) = &m_window;
	}

	void DeviceInterface::CreateDepthBuffer(
		const UINT width, 
		const UINT height, 
		DepthBuffer* pDepthBuffer)
	{
		// Creating temporary depth buffer (quick fix for sprint goal)
		{

			ID3D11Texture2D* pDepthTexture = NULL;
			{
				D3D11_TEXTURE2D_DESC desc = { 0 };
				desc.Width = width;
				desc.Height = height;
				desc.MipLevels = 0;
				desc.ArraySize = 1;
				desc.Format = DXGI_FORMAT_R32_TYPELESS;
				desc.SampleDesc = { 1, 0 };
				desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
				desc.CPUAccessFlags = 0;
				desc.MiscFlags = 0;

				m_pDevice4->CreateTexture2D(
					&desc,
					NULL,
					&pDepthTexture);
			}

			if (pDepthTexture)
			{
				D3D11_DEPTH_STENCIL_VIEW_DESC desc = {};
				desc.Format = DXGI_FORMAT_D32_FLOAT;
				desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
				desc.Texture2D.MipSlice = 0;

				m_pDevice4->CreateDepthStencilView(
					pDepthTexture,
					&desc,
					&pDepthBuffer->pView);

				pDepthTexture->Release();
			}
		}
	}

	void DeviceInterface::CreatePipeline(
		const std::string& vertexShader,
		const std::string& pixelShader,
		GraphicsPipeline** ppPipeline)
	{
		m_pipelineArray.CreateGraphicsPipeline(
			m_pDevice4,
			vertexShader,
			pixelShader,
			ppPipeline);
	}

	void DeviceInterface::CreateDynamicBufferRegion(
		const UINT size,
		const void* pData,
		BufferRegion* pRegion)
	{
		BufferRegion region;

		CreateBufferRegion(
			BUFFER_CONSTANT_DYNAMIC,
			size,
			&region);

		if (pData)
		{
			m_context.CopyDataToRegion(pData, size, region);
		}

		pRegion->DataCount = region.DataCount;
		pRegion->DataLocation = region.DataLocation;
		pRegion->ID = region.ID;
		pRegion->Type = region.Type;
	}

	void DeviceInterface::CreateStaticBufferRegion(
		const UINT size,
		const void* pData,
		BufferRegion* pRegion)
	{
		BufferRegion region;

		CreateBufferRegion(
			BUFFER_CONSTANT_STATIC,
			size,
			&region);

		if (pData)
		{
			m_context.CopyDataToRegion(pData, size, region);
		}

		pRegion->DataCount = region.DataCount;
		pRegion->DataLocation = region.DataLocation;
		pRegion->ID = region.ID;
		pRegion->Type = region.Type;
	}

	void DeviceInterface::CreateIndexBufferRegion(
		const UINT size,
		const void* pData,
		BufferRegion* pRegion)
	{
		BufferRegion region;

		CreateBufferRegion(
			BUFFER_VERTEX_INDICES,
			size,
			&region);

		if (pData)
		{
			m_context.CopyDataToRegion(pData, size, region);
		}

		pRegion->DataCount		= region.DataCount;
		pRegion->DataLocation	= region.DataLocation;
		pRegion->ID				= region.ID;
		pRegion->Type			= region.Type;
	}

	bool DeviceInterface::CreateVertexBufferRegion(
		const UINT vertexCount,
		const void* pVertices,
		const void* pNormals,
		const void* pUVs,
		BufferRegion* pRegion)
	{
		struct float3
		{
			float x, y, z;
		};

		struct float2
		{
			float x, y;
		};

		if (!pVertices) return false;

		BufferRegion Vertices;
		BufferRegion Normals;
		BufferRegion UVs;

		{
			CreateBufferRegion(
				BUFFER_VERTEX_POSITION,
				sizeof(float3) * vertexCount,
				&Vertices);

			CreateBufferRegion(
				BUFFER_VERTEX_NORMAL,
				sizeof(float3) * vertexCount,
				&Normals);

			CreateBufferRegion(
				BUFFER_VERTEX_UV,
				sizeof(float2) * vertexCount,
				&UVs);
		}


		{
			m_context.CopyDataToRegion(
				pVertices,
				sizeof(float3) * vertexCount,
				Vertices);

			if (pNormals)
			{
				m_context.CopyDataToRegion(
					pNormals,
					sizeof(float3) * vertexCount,
					Normals);
			}

			if (pUVs)
			{
				m_context.CopyDataToRegion(
					pUVs,
					sizeof(float2) * vertexCount,
					UVs);
			}
		}

		pRegion->DataCount		= Vertices.DataCount;
		pRegion->DataLocation	= Vertices.DataLocation;
		pRegion->ID				= Vertices.ID;
		pRegion->Type			= BUFFER_TYPE_UNKNOWN;

		return true;
	}

	void DeviceInterface::DeletePipeline(GraphicsPipeline* pPipeline)
	{
		m_pipelineArray.DeleteGraphicsPipeline(pPipeline);
	}

	void DeviceInterface::DeleteRenderTarget(const RenderTarget& renderTarget)
	{
		renderTarget.pView->Release();
	}

	void DeviceInterface::DeleteDepthBuffer(const DepthBuffer& depthBuffer)
	{
		depthBuffer.pView->Release();
	}
}