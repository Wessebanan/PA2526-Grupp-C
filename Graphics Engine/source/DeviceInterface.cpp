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
			CreateDXGIFactory2(
				DXGI_CREATE_FACTORY_DEBUG,
				IID_PPV_ARGS(&m_pFactory6));

			m_pFactory6->EnumAdapterByGpuPreference(
				0,
				DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
				IID_PPV_ARGS(&m_pAdapter4));

			ID3D11Device* pDeviceTemp = NULL;
			ID3D11DeviceContext* pContextTemp = NULL;

			UINT flag = D3D11_CREATE_DEVICE_DEBUG;

			D3D11CreateDevice(
				m_pAdapter4,
				D3D_DRIVER_TYPE_UNKNOWN,
				NULL,
				flag,
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

		if (m_backBuffer.m_pRenderTarget)
		{
			m_backBuffer.m_pRenderTarget->Release();
		}

		m_context.Release();
		m_storage.Release();

		m_pDevice4->Release();
	}

	void DeviceInterface::CreatePresentWindow(
		const UINT width,
		const UINT height,
		const char* pTitle,
		PresentWindow** ppWindow)
	{
		{
			m_window.Initialize(
				m_pDevice4,
				m_pFactory6,
				width,
				height,
				pTitle);

			m_window.m_pSwapChain4->GetBuffer(
				0,
				IID_PPV_ARGS(&m_backBuffer.m_pTexture2D));

			D3D11_RENDER_TARGET_VIEW_DESC desc = {};
			desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
			desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MipSlice = 0;

			m_pDevice4->CreateRenderTargetView(
				m_backBuffer.m_pTexture2D,
				&desc,
				&m_backBuffer.m_pRenderTarget);

			m_backBuffer.m_pTexture2D->Release();
		}

		(*ppWindow) = &m_window;

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

			if(pDepthTexture)
			{
				D3D11_DEPTH_STENCIL_VIEW_DESC desc = {};
				desc.Format = DXGI_FORMAT_D32_FLOAT;
				desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
				desc.Texture2D.MipSlice = 0;

				m_pDevice4->CreateDepthStencilView(
					pDepthTexture, 
					&desc,
					&m_context.m_pDepthBuffer);
			}
		}
	}

	RenderContext* DeviceInterface::QueryRenderContext()
	{
		return &m_context;
	}

	Texture2DView* DeviceInterface::QueryBackBuffer()
	{
		return &m_backBuffer;
	}

	void DeviceInterface::CreatePipeline(
		const std::string& vertexShader,
		const std::string& pixelShader,
		GraphicsPipeline** ppPipeline)
	{
		m_pipeline.Initialize(
			m_pDevice4,
			vertexShader,
			pixelShader);

		(*ppPipeline) = &m_pipeline;
	}

	void DeviceInterface::CreateDynamicBufferRegion(
		const UINT size,
		BufferRegion* pRegion)
	{
		CreateBufferRegion(
			BUFFER_CONSTANT_DYNAMIC,
			size,
			pRegion);
	}

	void DeviceInterface::CreateStaticBufferRegion(
		const UINT size, 
		BufferRegion* pRegion)
	{
		CreateBufferRegion(
			BUFFER_CONSTANT_STATIC,
			size,
			pRegion);
	}

	void DeviceInterface::CreateIndexBufferRegion(
		const UINT size, 
		BufferRegion* pRegion)
	{
		CreateBufferRegion(
			BUFFER_INDEX,
			size,
			pRegion);
	}

	bool DeviceInterface::CreateMeshRegion(
		const UINT vertexCount,
		const void* pVertices,
		const void* pNormals,
		const void* pUVs,
		BufferRegion* pRegion)
	{
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
		pRegion->Type			= BUFFER_TYPE_UNKNOWN;

		return true;
	}

	void DeviceInterface::DeletePipeline(GraphicsPipeline* pPipeline)
	{
		pPipeline->Release();
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
}