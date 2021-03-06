
#include "../includes/GraphicsInterface.h"
#include <fstream>

namespace graphics
{
	HRESULT CompileShader(
		const char* pShaderCode,
		const UINT length,
		const char* pTarget,
		ID3DBlob** ppBlob)
	{
		HRESULT hr;
		ID3DBlob* pErrorBlob = NULL;

		UINT compile_flag = 0;

#ifdef _DEBUG
		compile_flag = D3DCOMPILE_DEBUG;

#else // _RELEASE
		compile_flag = D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

		hr = D3DCompile(
			pShaderCode,
			length,
			NULL,
			NULL,
			NULL,
			"main",
			pTarget,
			compile_flag,
			0,
			ppBlob,
			&pErrorBlob);

		if (pErrorBlob)
		{
			OutputDebugStringA("COMPILING SHADER ERROR: ");
			OutputDebugStringA(pTarget);
			OutputDebugStringA(" : ");
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
			pErrorBlob->Release();
		}

		return hr;
	}

	HRESULT LoadCompiledShader(const char* pFilepath, uint8_t** ppData, size_t* pDataSize)
	{
		std::ifstream fs;

		fs.open(pFilepath, std::ifstream::in | std::ifstream::binary);
		if (!fs.good()) return E_INVALIDARG;

		struct stat filestatus;
		stat(pFilepath, &filestatus);
		size_t shader_code_size = filestatus.st_size;

		*pDataSize = shader_code_size;
		*ppData = (uint8_t*)malloc(shader_code_size);

		fs.seekg(0, std::ios::beg);
		fs.read(reinterpret_cast<char*>(*ppData), shader_code_size);
		fs.close();

		return S_OK;
	}


	HRESULT CreateVertexBuffer(
		ID3D11Device4* pDevice4, 
		const void* pData, 
		const UINT size, 
		const UINT stride,
		const UINT cpuAccess,
		const D3D11_USAGE usage,
		ID3D11Buffer** ppBuffer, 
		ID3D11ShaderResourceView** ppBufferView)
	{
		HRESULT hr;

		// Create Data Location
		{
			D3D11_BUFFER_DESC buffer_desc = { 0 };
			buffer_desc.BindFlags			= D3D11_BIND_SHADER_RESOURCE;
			buffer_desc.ByteWidth			= size;
			buffer_desc.Usage				= usage;
			buffer_desc.CPUAccessFlags		= cpuAccess;
			buffer_desc.MiscFlags			= D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			buffer_desc.StructureByteStride = stride;

			if (pData)
			{
				D3D11_SUBRESOURCE_DATA data = { 0 };
				data.pSysMem = pData;
				hr = pDevice4->CreateBuffer(&buffer_desc, &data, ppBuffer);
			}
			else
			{
				hr = pDevice4->CreateBuffer(&buffer_desc, NULL, ppBuffer);
			}

			if (FAILED(hr)) return hr;
		}

		assert(ppBuffer);

		// Create Data View
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC view_desc = {};
			view_desc.Format				= DXGI_FORMAT_UNKNOWN;
			view_desc.ViewDimension			= D3D11_SRV_DIMENSION_BUFFER;
			view_desc.Buffer.FirstElement	= 0;
			view_desc.Buffer.NumElements	= size / stride;

			hr = pDevice4->CreateShaderResourceView(*ppBuffer, &view_desc, ppBufferView);
		}

		return FAILED(hr) ? hr : S_OK;
	}

	HRESULT CreateVertexShader(
		ID3D11Device4* pDevice4,
		const char* pShaderCode,
		const UINT shaderLength,
		ID3D11VertexShader** ppVertexShader)
	{
		int result;
		HRESULT hr;

		// Compile Shader
		ID3DBlob* pShader = NULL;
		result = CompileShader(
			pShaderCode,
			shaderLength,
			"vs_5_0",
			&pShader);

		if (!result) return FALSE;

		// Create Vertex Shader With Compiled Code
		hr = pDevice4->CreateVertexShader(
			pShader->GetBufferPointer(),
			pShader->GetBufferSize(),
			NULL,
			ppVertexShader);

		pShader->Release();

		return hr;
	}

	HRESULT CreatePixelShader(
		ID3D11Device4* pDevice4, 
		const char* pShaderCode, 
		const UINT shaderLength, 
		ID3D11PixelShader** ppPixelShader)
	{
		int result;
		HRESULT hr;

		// Compile Shader
		ID3DBlob* pShader = NULL;
		result = CompileShader(
			pShaderCode,
			shaderLength,
			"ps_5_0",
			&pShader);

		if (!result) return FALSE;

		// Create Pixel Shader With Compiled Code
		hr = pDevice4->CreatePixelShader(
			pShader->GetBufferPointer(),
			pShader->GetBufferSize(),
			NULL,
			ppPixelShader);

		pShader->Release();

		return hr;
	}

	HRESULT CreateComputeShader(
		ID3D11Device4* pDevice4, 
		const char* pShaderCode, 
		const UINT shaderLength, 
		ID3D11ComputeShader** ppComputeShader)
	{
		int result;
		HRESULT hr;

		// Compile Shader
		ID3DBlob* pShader = NULL;
		result = CompileShader(
			pShaderCode,
			shaderLength,
			"cs_5_0",
			&pShader);

		if (!result) return FALSE;

		// Create Pixel Shader With Compiled Code
		hr = pDevice4->CreateComputeShader(
			pShader->GetBufferPointer(),
			pShader->GetBufferSize(),
			NULL,
			ppComputeShader);

		pShader->Release();

		return hr;
	}

	HRESULT CreateVertexShaderFromFile(
		ID3D11Device4* pDevice4,
		const char* pFilepath,
		ID3D11VertexShader** ppVertexShader)
	{
		HRESULT hr		= S_OK;
		size_t size		= 0;
		uint8_t* pData	= NULL;

		hr = LoadCompiledShader(pFilepath, &pData, &size);
		if (FAILED(hr)) return hr;

		hr = pDevice4->CreateVertexShader(
			pData,
			size,
			nullptr,
			ppVertexShader);

		free(pData);

		return hr;
	}

	HRESULT CreatePixelShaderFromFile(
		ID3D11Device4* pDevice4, 
		const char* pFilepath, 
		ID3D11PixelShader** ppPixelShader)
	{
		HRESULT hr		= S_OK;
		size_t size		= 0;
		uint8_t* pData	= NULL;

		hr = LoadCompiledShader(pFilepath, &pData, &size);
		if (FAILED(hr)) return hr;

		hr = pDevice4->CreatePixelShader(
			pData,
			size,
			nullptr,
			ppPixelShader);

		free(pData);

		return hr;
	}

	HRESULT CreateComputeShaderFromFile(
		ID3D11Device4* pDevice4, 
		const char* pFilepath, 
		ID3D11ComputeShader** ppComputeShader)
	{
		HRESULT hr		= S_OK;
		size_t size		= 0;
		uint8_t* pData	= NULL;

		hr = LoadCompiledShader(pFilepath, &pData, &size);
		if (FAILED(hr)) return hr;

		hr = pDevice4->CreateComputeShader(
			pData,
			size,
			nullptr,
			ppComputeShader);

		free(pData);

		return hr;
	}

	namespace internal
	{
		static ID3D11Device4* gpDevice4					= NULL;
		static ID3D11DeviceContext4* gpDeviceContext4	= NULL;
		
		static IDXGIFactory6* gpFactory6				= NULL;
		static IDXGIAdapter4* gpAdapter4				= NULL;
		
		static IDXGISwapChain4* gpSwapChain4			= NULL;
		static ID3D11RenderTargetView* gpBackBuffer		= NULL;
	
		static bool gIsActive							= false;

		HRESULT CreateSwapChain(const HWND hWnd)
		{
			HRESULT hr = S_OK;

			{
				IDXGISwapChain1* pTemp = NULL;

				DXGI_SWAP_CHAIN_DESC1 desc = { 0 };
				desc.Width = 0;
				desc.Height = 0;
				desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				desc.Stereo = FALSE;
				desc.SampleDesc = { 1, 0 };
				desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
				desc.BufferCount = 2;
				desc.Scaling = DXGI_SCALING_NONE;
				desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
				desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
				desc.Flags = 0;

				hr = gpFactory6->CreateSwapChainForHwnd(
					gpDevice4,
					hWnd,
					&desc,
					NULL,
					NULL,
					&pTemp);

				if (FAILED(hr)) return hr;

				pTemp->QueryInterface(IID_PPV_ARGS(&gpSwapChain4));
				pTemp->Release();

				gpFactory6->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);
			}

			{
				ID3D11Texture2D* pTexture = NULL;

				hr = gpSwapChain4->GetBuffer(
					0,
					IID_PPV_ARGS(&pTexture));

				D3D11_RENDER_TARGET_VIEW_DESC desc = {};
				desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
				desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
				desc.Texture2D.MipSlice = 0;

				if (!pTexture) return E_INVALIDARG;

				gpDevice4->CreateRenderTargetView(
					pTexture,
					&desc,
					&gpBackBuffer);

				pTexture->Release();
			}

			return hr;
		}

		void GetBackBuffer(ID3D11RenderTargetView** ppBackBuffer)
		{
			*ppBackBuffer = gpBackBuffer;
		}

		void GetSwapChain(IDXGISwapChain1** ppSwapChain)
		{
			*ppSwapChain = gpSwapChain4;
		}

		void GetD3D11(D3D11_DEVICE_HANDLE* pHandle)
		{
			pHandle->pDevice4			= gpDevice4;
			pHandle->pDeviceContext4	= gpDeviceContext4;
			pHandle->pFactory6			= gpFactory6;
			pHandle->pAdapter4			= gpAdapter4;
		}

		HRESULT CreateAndSetInputLayout()
		{
			HRESULT hr;
			ID3D11InputLayout* pInputLayout = NULL;

			const std::string gDummyVertexShader = R"(
			float4 main(
				uint VertexID : VertexStart, 
				uint InstanceID : InstanceStart) : SV_POSITION
			{
				return float4(1.0f, 1.0f, 1.0f, 1.0f);
			}

			)";

			ID3DBlob* pVSBlob = NULL;

			hr = graphics::CompileShader(
				gDummyVertexShader.c_str(),
				(UINT)gDummyVertexShader.length(),
				"vs_5_0",
				&pVSBlob);

			if (FAILED(hr)) return hr;

			D3D11_INPUT_ELEMENT_DESC element_desc[2];
			element_desc[0].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			element_desc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;

			element_desc[0].Format = DXGI_FORMAT_R32_UINT;
			element_desc[0].InputSlot = 0;
			element_desc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			element_desc[0].InstanceDataStepRate = 0;
			element_desc[0].SemanticIndex = 0;
			element_desc[0].SemanticName = "VertexStart";

			element_desc[1].Format = DXGI_FORMAT_R32_UINT;
			element_desc[1].InputSlot = 1;
			element_desc[1].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
			element_desc[1].InstanceDataStepRate = 1;
			element_desc[1].SemanticIndex = 0;
			element_desc[1].SemanticName = "InstanceStart";

			hr = gpDevice4->CreateInputLayout(
				element_desc,
				2,
				pVSBlob->GetBufferPointer(),
				pVSBlob->GetBufferSize(),
				&pInputLayout);

			if (FAILED(hr))
			{
				pVSBlob->Release();
				return hr;
			}

			gpDeviceContext4->IASetInputLayout(pInputLayout);

			pInputLayout->Release();
			pVSBlob->Release();

			return S_OK;
		}

		HRESULT CreateAndSetVertexBuffers(
			ID3D11Device4* pDevice4, 
			ID3D11DeviceContext4* pContext4,
			const UINT maximumVertexCountPerDraw)
		{
			HRESULT hr;

			// maximum indices in vertex shader (vertex data and instance data)
			const UINT maximum_indices = maximumVertexCountPerDraw;

			ID3D11Buffer* pBuffer = NULL;
			{
				UINT* pIndices = new UINT[maximum_indices];
				for (UINT i = 0; i < maximum_indices; i++)
				{
					pIndices[i] = i;
				}

				D3D11_BUFFER_DESC desc = { 0 };
				desc.BindFlags	= D3D11_BIND_VERTEX_BUFFER;
				desc.ByteWidth	= maximum_indices * sizeof(UINT);
				desc.Usage		= D3D11_USAGE_IMMUTABLE;

				D3D11_SUBRESOURCE_DATA data = { 0 };
				data.pSysMem = pIndices;

				hr = pDevice4->CreateBuffer(&desc, &data, &pBuffer);
				delete[] pIndices;

				if (FAILED(hr)) return hr;
			}

			ID3D11Buffer* pVertexBuffers[2] =
			{
				pBuffer,
				pBuffer
			};

			UINT strides[2] =
			{
				sizeof(UINT),
				sizeof(UINT),
			};

			UINT offsets[2] =
			{
				0,
				0,
			};

			pContext4->IASetVertexBuffers(0, 2, pVertexBuffers, strides, offsets);
			pBuffer->Release();

			return S_OK;
		}
	}

	HRESULT InitializeD3D11()
	{
		if (internal::gIsActive) return S_OK;

		HRESULT hr = S_OK;
		UINT device_flag = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
		UINT factory_flag = 0;

#ifdef _DEBUG
		device_flag |= D3D11_CREATE_DEVICE_DEBUG;
		factory_flag |= DXGI_CREATE_FACTORY_DEBUG;
#endif // _DEBUG

		hr = CreateDXGIFactory2(
			factory_flag,
			IID_PPV_ARGS(&internal::gpFactory6));

		if (FAILED(hr)) return hr;

		hr = internal::gpFactory6->EnumAdapterByGpuPreference(
			0,
			DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
			IID_PPV_ARGS(&internal::gpAdapter4));

		if (FAILED(hr))
		{
			internal::gpFactory6->Release();
			return hr;
		}

		ID3D11Device* pDeviceTemp = NULL;
		ID3D11DeviceContext* pContextTemp = NULL;

		D3D11CreateDevice(
			internal::gpAdapter4,
			D3D_DRIVER_TYPE_UNKNOWN,
			NULL,
			device_flag,
			NULL,
			0,
			D3D11_SDK_VERSION,
			&pDeviceTemp,
			NULL,
			&pContextTemp);

		if (FAILED(hr))
		{
			internal::gpFactory6->Release();
			internal::gpAdapter4->Release();
			return hr;
		}

		pDeviceTemp->QueryInterface(IID_PPV_ARGS(&internal::gpDevice4));
		pContextTemp->QueryInterface(IID_PPV_ARGS(&internal::gpDeviceContext4));

		pDeviceTemp->Release();
		pContextTemp->Release();

		hr = internal::CreateAndSetInputLayout();
		if (FAILED(hr)) return hr;

		internal::gIsActive = true;
		return hr;
	}

	void DestroyD3D11()
	{
		if (internal::gIsActive)
		{
			internal::gIsActive = false;

			SafeRelease((IUnknown**)& internal::gpBackBuffer);
			SafeRelease((IUnknown**)& internal::gpSwapChain4);

			SafeRelease((IUnknown**)& internal::gpFactory6);
			SafeRelease((IUnknown**)& internal::gpAdapter4);

			SafeRelease((IUnknown**)& internal::gpDevice4);
			SafeRelease((IUnknown**)& internal::gpDeviceContext4);
		}
	}

	HRESULT AttachHwndToSwapChain(const HWND hWnd)
	{
		return internal::CreateSwapChain(hWnd);
	}

	void Present(const UINT syncInterval)
	{
		internal::gpSwapChain4->Present(syncInterval, 0);
	}
}