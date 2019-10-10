#include "../../includes/graphics/DeviceInterface.h"

namespace graphics
{
	DeviceInterface::DeviceInterface()
	{
		m_pAdapter4		= NULL;
		m_pDevice4		= NULL;
		m_pFactory6		= NULL;

		ZeroMemory(m_pSamplerStates, sizeof(m_pSamplerStates));
	}

	DeviceInterface::~DeviceInterface()
	{
	}

	int DeviceInterface::Initialize()
	{
		HRESULT hr;
		{
			UINT factoryFlag	= 0;
			UINT deviceFlag		= 0;

#ifdef _DEBUG
			deviceFlag	|= D3D11_CREATE_DEVICE_DEBUG;
			factoryFlag |= DXGI_CREATE_FACTORY_DEBUG;
#endif // _DEBUG

			hr = CreateDXGIFactory2(
				factoryFlag,
				IID_PPV_ARGS(&m_pFactory6));

			if (FAILED(hr)) return FALSE;

			hr = m_pFactory6->EnumAdapterByGpuPreference(
				0,
				DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
				IID_PPV_ARGS(&m_pAdapter4));

			if (FAILED(hr))
			{
				m_pFactory6->Release();
				return FALSE;
			}

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

			if (FAILED(hr))
			{
				m_pFactory6->Release();
				m_pAdapter4->Release();
				return FALSE;
			}

			pDeviceTemp->QueryInterface(IID_PPV_ARGS(&m_pDevice4));
			pContextTemp->QueryInterface(IID_PPV_ARGS(&m_context.m_pContext4));

			pDeviceTemp->Release();
			pContextTemp->Release();
		}

		int flag;

		flag = m_storage.Initialize(m_pDevice4);
		if (!flag) return FALSE;

		flag = m_context.Initialize(m_pDevice4, &m_storage);
		if (!flag) return FALSE;

		//flag = m_pipelineArray.Initialize(m_pDevice4, MAXIMUM_GRAPHICS_PIPELINES);
		//if (!flag) return FALSE;

		{
			D3D11_SAMPLER_DESC desc = {};

			desc.Filter			= D3D11_FILTER_COMPARISON_ANISOTROPIC;
			desc.AddressU		= D3D11_TEXTURE_ADDRESS_BORDER;
			desc.AddressV		= D3D11_TEXTURE_ADDRESS_BORDER;
			desc.AddressW		= D3D11_TEXTURE_ADDRESS_BORDER;
			desc.MaxAnisotropy	= 16;
			desc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
			desc.BorderColor[0] = 1.0f;
			desc.BorderColor[1] = 1.0f;
			desc.BorderColor[2] = 1.0f;
			desc.BorderColor[3] = 1.0f;

			m_pDevice4->CreateSamplerState(&desc, &m_pSamplerStates[0]);
			m_context.m_pContext4->PSSetSamplers(0, 1, &m_pSamplerStates[0]);
		}

		return TRUE;
	}


	void DeviceInterface::Release()
	{
		m_pAdapter4->Release();
		m_pFactory6->Release();

		m_context.Release();
		m_storage.Release();

		m_pSamplerStates[0]->Release();

		m_pDevice4->Release();
	}

	RenderContext* DeviceInterface::GetRenderContext()
	{
		return &m_context;
	}

	//UINT64 DeviceInterface::QueryVideoMemoryUsage()
	//{
	//	DXGI_QUERY_VIDEO_MEMORY_INFO info;

	//	m_pAdapter4->QueryVideoMemoryInfo(
	//		0,
	//		DXGI_MEMORY_SEGMENT_GROUP_LOCAL,
	//		&info);

	//	return info.CurrentUsage;
	//}

	int DeviceInterface::CreatePresentWindow(
		const UINT width, 
		const UINT height, 
		const char* pTitle,
		RenderTarget* pRenderTarget,
		PresentWindow* pWindow)
	{
		HRESULT hr = S_OK;
		{
			hr = pWindow->Initialize(
				m_pDevice4,
				m_pFactory6,
				width,
				height,
				pTitle);

			if (FAILED(hr)) return FALSE;

			ID3D11Texture2D* pTexture2D = NULL;
			hr = pWindow->m_pSwapChain4->GetBuffer(
				0,
				IID_PPV_ARGS(&pTexture2D));

			if (FAILED(hr)) return FALSE;

			if (pTexture2D)
			{
				// setting region type to unknow
				// swap chain is the owner
				pRenderTarget->Region.Type = BUFFER_TYPE_UNKNOWN;

				D3D11_RENDER_TARGET_VIEW_DESC desc = {};
				desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
				desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
				desc.Texture2D.MipSlice = 0;

				hr = m_pDevice4->CreateRenderTargetView(
					pTexture2D,
					&desc,
					&pRenderTarget->pView);

				pTexture2D->Release();

				if (FAILED(hr)) return FALSE;
			}
		}

		return TRUE;
	}

	int DeviceInterface::CreateDepthBuffer(
		const UINT width, 
		const UINT height, 
		DepthBuffer* pDepthBuffer,
		const bool createShaderResource)
	{
		// Creating temporary depth buffer (quick fix for sprint goal)
		HRESULT hr = S_OK;
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

				if (createShaderResource)
					desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;

				desc.CPUAccessFlags = 0;
				desc.MiscFlags = 0;

				hr = m_pDevice4->CreateTexture2D(
					&desc,
					NULL,
					&pDepthTexture);

				if (FAILED(hr)) return FALSE;
			}

			if (pDepthTexture)
			{
				if (createShaderResource)
				{
					D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
					srv_desc.Format			= DXGI_FORMAT_R32_FLOAT;
					srv_desc.ViewDimension	= D3D11_SRV_DIMENSION_TEXTURE2D;
					srv_desc.Texture2D.MipLevels = -1;
					srv_desc.Texture2D.MostDetailedMip = 0;

					m_pDevice4->CreateShaderResourceView(
						pDepthTexture, 
						&srv_desc,
						&pDepthBuffer->pResource);
				}

				D3D11_DEPTH_STENCIL_VIEW_DESC desc = {};
				desc.Format = DXGI_FORMAT_D32_FLOAT;
				desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
				desc.Texture2D.MipSlice = 0;

				hr = m_pDevice4->CreateDepthStencilView(
					pDepthTexture,
					&desc,
					&pDepthBuffer->pView);

				pDepthTexture->Release();

				if (FAILED(hr)) return FALSE;
			}
		}

		return TRUE;
	}

	int DeviceInterface::CreateGraphicsPipeline(
		const std::string& vertexShader,
		const std::string& pixelShader,
		GraphicsPipeline* pPipeline)
	{
		return pPipeline->Initialize(m_pDevice4, vertexShader, pixelShader);

		//return m_pipelineArray.CreateGraphicsPipeline(
		//	m_pDevice4,
		//	vertexShader,
		//	pixelShader,
		//	ppPipeline);
	}

	int DeviceInterface::CreateBufferRegion(
		const BUFFER_TYPE type,
		const UINT size,
		const void* pData,
		BufferRegion* pRegion)
	{
		// Get the cpu buffer and allocate a region on it
		BufferHeap* pHeap = m_storage.GetBufferHeapCPU(type);
		int result = pHeap->AllocateRegion(size, pRegion);

		if (!result) return FALSE;

		// copy data to the region if it exists
		if (pData)
		{
			m_context.CopyDataToRegion(pData, size, *pRegion);
		}

		return TRUE;
	}

	int DeviceInterface::CreateDynamicBufferRegion(
		const UINT size,
		const void* pData,
		BufferRegion* pRegion)
	{
		int result = CreateBufferRegion(
			BUFFER_CONSTANT_DYNAMIC,
			size,
			pData,
			pRegion);

		return TRUE;
	}

	int DeviceInterface::CreateStaticBufferRegion(
		const UINT size,
		const void* pData,
		BufferRegion* pRegion)
	{
		int result = CreateBufferRegion(
			BUFFER_CONSTANT_STATIC,
			size,
			pData,
			pRegion);

		return TRUE;
	}

	int DeviceInterface::CreateIndexBufferRegion(
		const UINT indexCount,
		const void* pData,
		BufferRegion* pRegion)
	{
		int result = CreateBufferRegion(
			BUFFER_VERTEX_INDICES,
			indexCount * sizeof(int),
			pData,
			pRegion);

		return TRUE;
	}

	int DeviceInterface::CreateVertexBufferRegion(
		const UINT vertexCount,
		const void* pVertices,
		const void* pNormals,
		const void* pUVs,
		const void* pBlendWeights,
		const void* pBlendIndices,
		BufferRegion* pRegion)
	{
		// Vertex buffer is a bit different since it 
		// must handle several buffers within one

		// allocate separate regions on the 3 buffers
		// and let one be primary (since they are the same)

		// memory will be inefficent because of this 
		// can be improved later

		struct float3
		{
			float x, y, z;
		};

		struct float2
		{
			float x, y;
		};

		struct int4
		{
			int x, y, z, w;
		};

		if (!pVertices) return FALSE;

		BufferRegion Vertices;
		BufferRegion Normals;
		BufferRegion UVs;

		BufferRegion BlendWeights;
		BufferRegion BlendIndices;

		int result;
		{
			result = CreateBufferRegion(
				BUFFER_VERTEX_POSITION,
				sizeof(float3) * vertexCount,
				pVertices,
				&Vertices);

			if (!result) return FALSE;

			result = CreateBufferRegion(
				BUFFER_VERTEX_NORMAL,
				sizeof(float3) * vertexCount,
				pNormals,
				&Normals);

			if (!result) return FALSE;

			result = CreateBufferRegion(
				BUFFER_VERTEX_UV,
				sizeof(float2) * vertexCount,
				pUVs,
				&UVs);

			if (!result) return FALSE;

			result = CreateBufferRegion(
				BUFFER_VERTEX_BLEND_WEIGHT,
				sizeof(float3) * vertexCount,
				pBlendWeights,
				&BlendWeights);

			if (!result) return FALSE;

			result = CreateBufferRegion(
				BUFFER_VERTEX_BLEND_INDICES,
				sizeof(int4) * vertexCount,
				pBlendIndices,
				&BlendIndices);

			if (!result) return FALSE;
		}

		pRegion->DataCount		= Vertices.DataCount;
		pRegion->DataLocation	= Vertices.DataLocation;
		pRegion->ID				= Vertices.ID;
		pRegion->Type			= BUFFER_TYPE_UNKNOWN;

		return TRUE;
	}

	void DeviceInterface::DeleteGraphicsPipeline(GraphicsPipeline* pPipeline)
	{
		pPipeline->Release();
	}

	void DeviceInterface::DeleteRenderTarget(const RenderTarget& renderTarget)
	{
		renderTarget.pView->Release();
	}

	void DeviceInterface::DeleteDepthBuffer(const DepthBuffer& depthBuffer)
	{
		depthBuffer.pView->Release();

		if (depthBuffer.pResource)
			depthBuffer.pResource->Release();
	}

	void DeviceInterface::DeletePresentWindow(const PresentWindow& window)
	{
		window.m_pSwapChain4->Release();
	}
}