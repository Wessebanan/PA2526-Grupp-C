#pragma once

#include "D3D11Header.h"

namespace graphics
{
	inline void SafeRelease(IUnknown** ppUnknown)
	{
		if (*ppUnknown)
		{
			(*ppUnknown)->Release();
			(*ppUnknown) = NULL;
		}
	}

	HRESULT CompileShader(
		const char* pShaderCode,
		const UINT length,
		const char* pTarget,
		ID3DBlob** ppBlob);

	HRESULT CreateVertexBuffer(
		ID3D11Device4* pDevice4,
		const void* pData,
		const UINT size,
		const UINT stride,
		const UINT cpuAccess,
		const D3D11_USAGE usage,
		ID3D11Buffer** ppBuffer,
		ID3D11ShaderResourceView** ppBufferView);

	HRESULT CreateVertexShader(
		ID3D11Device4* pDevice4,
		const char* pShaderCode,
		const UINT shaderLength,
		ID3D11VertexShader** ppVertexShader);

	HRESULT CreatePixelShader(
		ID3D11Device4* pDevice4,
		const char* pShaderCode,
		const UINT shaderLength,
		ID3D11PixelShader** ppPixelShader);

	HRESULT CreateComputeShader(
		ID3D11Device4* pDevice4,
		const char* pShaderCode,
		const UINT shaderLength,
		ID3D11ComputeShader** ppComputeShader);

	HRESULT CreateVertexShaderFromFile(
		ID3D11Device4* pDevice4,
		const char* pFilepath,
		ID3D11VertexShader** ppVertexShader);

	HRESULT CreatePixelShaderFromFile(
		ID3D11Device4* pDevice4,
		const char* pFilepath,
		ID3D11PixelShader** ppPixelShader);

	HRESULT CreateComputeShaderFromFile(
		ID3D11Device4* pDevice4,
		const char* pFilepath,
		ID3D11ComputeShader** ppComputeShader);

	namespace window
	{
		struct CLIENT_INT2
		{
			int x, y;
		};

		inline CLIENT_INT2 GetWorkAreaResolution()
		{
			RECT r;
			SystemParametersInfoA(SPI_GETWORKAREA, 0, &r, 0);

			return { 
				r.right - r.left, 
				r.bottom - r.top 
			};
		}

		inline CLIENT_INT2 GetDisplayResolution()
		{
			return { 
				GetSystemMetrics(SM_CXSCREEN), 
				GetSystemMetrics(SM_CYSCREEN) 
			};
		}

		inline CLIENT_INT2 GetClientResolution(const HWND hWnd)
		{
			RECT r;
			GetClientRect(hWnd, &r);

			return { 
				r.right - r.left, 
				r.bottom - r.top 
			};
		}
	}

	namespace internal
	{
		HRESULT InitializeD3D11();

		HRESULT CreateSwapChain(HWND hWnd);
		void GetBackBuffer(ID3D11RenderTargetView** ppBackBuffer);
		void Present(const UINT syncInterval);

		void DestroyD3D11();

		struct D3D11_DEVICE_HANDLE
		{
			ID3D11Device4* pDevice4;
			ID3D11DeviceContext4* pDeviceContext4;
			
			IDXGIFactory6* pFactory6;
			IDXGIAdapter4* pAdapter4;
		};

		void GetD3D11(D3D11_DEVICE_HANDLE* pHandle);

		HRESULT CreateAndSetInputLayout(
			ID3D11Device4* pDevice4,
			ID3D11DeviceContext4* pContext4);

		HRESULT CreateAndSetVertexBuffers(
			ID3D11Device4* pDevice4,
			ID3D11DeviceContext4* pContext4);
	}
}