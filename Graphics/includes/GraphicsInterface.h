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

	HRESULT CreateSwapChain(
		ID3D11Device4* pDevice4,
		IDXGIFactory6* pFactory6,
		HWND hWnd,
		IDXGISwapChain4** ppSwapChain4);

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

	HRESULT CreateVertexShaderFromFile(
		ID3D11Device4* pDevice4,
		const char* pFilepath,
		ID3D11VertexShader** ppVertexShader);

	HRESULT CreatePixelShader(
		ID3D11Device4* pDevice4,
		const char* pShaderCode,
		const UINT shaderLength,
		ID3D11PixelShader** ppPixelShader);

	HRESULT CreatePixelShaderFromFile(
		ID3D11Device4* pDevice4,
		const char* pFilepath,
		ID3D11PixelShader** ppPixelShader);

	namespace internal
	{
		HRESULT InitializeD3D11();

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

		HRESULT Initialize(
			ID3D11Device4* pDevice4,
			ID3D11DeviceContext4* pContext4);
	}
}