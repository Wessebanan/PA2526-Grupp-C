#include "ForwardRenderingPipeline.h"

#include <wincodec.h>
#pragma comment(lib, "Windowscodecs.lib")

namespace graphics
{
	// get the dxgi format equivilent of a wic format
	static DXGI_FORMAT GetDXGIFormatFromWICFormat(WICPixelFormatGUID& wicFormatGUID)
	{
		if (wicFormatGUID == GUID_WICPixelFormat128bppRGBAFloat) return DXGI_FORMAT_R32G32B32A32_FLOAT;
		else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBAHalf) return DXGI_FORMAT_R16G16B16A16_FLOAT;
		else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBA) return DXGI_FORMAT_R16G16B16A16_UNORM;
		else if (wicFormatGUID == GUID_WICPixelFormat32bppRGBA) return DXGI_FORMAT_R8G8B8A8_UNORM;
		else if (wicFormatGUID == GUID_WICPixelFormat32bppBGRA) return DXGI_FORMAT_B8G8R8A8_UNORM;
		else if (wicFormatGUID == GUID_WICPixelFormat32bppBGR) return DXGI_FORMAT_B8G8R8X8_UNORM;
		else if (wicFormatGUID == GUID_WICPixelFormat32bppRGBA1010102XR) return DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM;

		else if (wicFormatGUID == GUID_WICPixelFormat32bppRGBA1010102) return DXGI_FORMAT_R10G10B10A2_UNORM;
		else if (wicFormatGUID == GUID_WICPixelFormat16bppBGRA5551) return DXGI_FORMAT_B5G5R5A1_UNORM;
		else if (wicFormatGUID == GUID_WICPixelFormat16bppBGR565) return DXGI_FORMAT_B5G6R5_UNORM;
		else if (wicFormatGUID == GUID_WICPixelFormat32bppGrayFloat) return DXGI_FORMAT_R32_FLOAT;
		else if (wicFormatGUID == GUID_WICPixelFormat16bppGrayHalf) return DXGI_FORMAT_R16_FLOAT;
		else if (wicFormatGUID == GUID_WICPixelFormat16bppGray) return DXGI_FORMAT_R16_UNORM;
		else if (wicFormatGUID == GUID_WICPixelFormat8bppGray) return DXGI_FORMAT_R8_UNORM;
		else if (wicFormatGUID == GUID_WICPixelFormat8bppAlpha) return DXGI_FORMAT_A8_UNORM;

		return DXGI_FORMAT_UNKNOWN;
	}

	// get a dxgi compatible wic format from another wic format
	static WICPixelFormatGUID GetConvertToWICFormat(WICPixelFormatGUID& wicFormatGUID)
	{
		if (wicFormatGUID == GUID_WICPixelFormatBlackWhite) return GUID_WICPixelFormat8bppGray;
		else if (wicFormatGUID == GUID_WICPixelFormat1bppIndexed) return GUID_WICPixelFormat32bppRGBA;
		else if (wicFormatGUID == GUID_WICPixelFormat2bppIndexed) return GUID_WICPixelFormat32bppRGBA;
		else if (wicFormatGUID == GUID_WICPixelFormat4bppIndexed) return GUID_WICPixelFormat32bppRGBA;
		else if (wicFormatGUID == GUID_WICPixelFormat8bppIndexed) return GUID_WICPixelFormat32bppRGBA;
		else if (wicFormatGUID == GUID_WICPixelFormat2bppGray) return GUID_WICPixelFormat8bppGray;
		else if (wicFormatGUID == GUID_WICPixelFormat4bppGray) return GUID_WICPixelFormat8bppGray;
		else if (wicFormatGUID == GUID_WICPixelFormat16bppGrayFixedPoint) return GUID_WICPixelFormat16bppGrayHalf;
		else if (wicFormatGUID == GUID_WICPixelFormat32bppGrayFixedPoint) return GUID_WICPixelFormat32bppGrayFloat;
		else if (wicFormatGUID == GUID_WICPixelFormat16bppBGR555) return GUID_WICPixelFormat16bppBGRA5551;
		else if (wicFormatGUID == GUID_WICPixelFormat32bppBGR101010) return GUID_WICPixelFormat32bppRGBA1010102;
		else if (wicFormatGUID == GUID_WICPixelFormat24bppBGR) return GUID_WICPixelFormat32bppRGBA;
		else if (wicFormatGUID == GUID_WICPixelFormat24bppRGB) return GUID_WICPixelFormat32bppRGBA;
		else if (wicFormatGUID == GUID_WICPixelFormat32bppPBGRA) return GUID_WICPixelFormat32bppRGBA;
		else if (wicFormatGUID == GUID_WICPixelFormat32bppPRGBA) return GUID_WICPixelFormat32bppRGBA;
		else if (wicFormatGUID == GUID_WICPixelFormat48bppRGB) return GUID_WICPixelFormat64bppRGBA;
		else if (wicFormatGUID == GUID_WICPixelFormat48bppBGR) return GUID_WICPixelFormat64bppRGBA;
		else if (wicFormatGUID == GUID_WICPixelFormat64bppBGRA) return GUID_WICPixelFormat64bppRGBA;
		else if (wicFormatGUID == GUID_WICPixelFormat64bppPRGBA) return GUID_WICPixelFormat64bppRGBA;
		else if (wicFormatGUID == GUID_WICPixelFormat64bppPBGRA) return GUID_WICPixelFormat64bppRGBA;
		else if (wicFormatGUID == GUID_WICPixelFormat48bppRGBFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
		else if (wicFormatGUID == GUID_WICPixelFormat48bppBGRFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
		else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBAFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
		else if (wicFormatGUID == GUID_WICPixelFormat64bppBGRAFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
		else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
		else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBHalf) return GUID_WICPixelFormat64bppRGBAHalf;
		else if (wicFormatGUID == GUID_WICPixelFormat48bppRGBHalf) return GUID_WICPixelFormat64bppRGBAHalf;
		else if (wicFormatGUID == GUID_WICPixelFormat128bppPRGBAFloat) return GUID_WICPixelFormat128bppRGBAFloat;
		else if (wicFormatGUID == GUID_WICPixelFormat128bppRGBFloat) return GUID_WICPixelFormat128bppRGBAFloat;
		else if (wicFormatGUID == GUID_WICPixelFormat128bppRGBAFixedPoint) return GUID_WICPixelFormat128bppRGBAFloat;
		else if (wicFormatGUID == GUID_WICPixelFormat128bppRGBFixedPoint) return GUID_WICPixelFormat128bppRGBAFloat;
		else if (wicFormatGUID == GUID_WICPixelFormat32bppRGBE) return GUID_WICPixelFormat128bppRGBAFloat;
		else if (wicFormatGUID == GUID_WICPixelFormat32bppCMYK) return GUID_WICPixelFormat32bppRGBA;
		else if (wicFormatGUID == GUID_WICPixelFormat64bppCMYK) return GUID_WICPixelFormat64bppRGBA;
		else if (wicFormatGUID == GUID_WICPixelFormat40bppCMYKAlpha) return GUID_WICPixelFormat64bppRGBA;
		else if (wicFormatGUID == GUID_WICPixelFormat80bppCMYKAlpha) return GUID_WICPixelFormat64bppRGBA;

#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8) || defined(_WIN7_PLATFORM_UPDATE)
		else if (wicFormatGUID == GUID_WICPixelFormat32bppRGB) return GUID_WICPixelFormat32bppRGBA;
		else if (wicFormatGUID == GUID_WICPixelFormat64bppRGB) return GUID_WICPixelFormat64bppRGBA;
		else if (wicFormatGUID == GUID_WICPixelFormat64bppPRGBAHalf) return GUID_WICPixelFormat64bppRGBAHalf;
#endif

		return GUID_WICPixelFormatDontCare;
	}

	// get the number of bits per pixel for a dxgi format
	static int GetDXGIFormatBitsPerPixel(DXGI_FORMAT& dxgiFormat)
	{
		if (dxgiFormat == DXGI_FORMAT_R32G32B32A32_FLOAT) return 128;
		else if (dxgiFormat == DXGI_FORMAT_R16G16B16A16_FLOAT) return 64;
		else if (dxgiFormat == DXGI_FORMAT_R16G16B16A16_UNORM) return 64;
		else if (dxgiFormat == DXGI_FORMAT_R8G8B8A8_UNORM) return 32;
		else if (dxgiFormat == DXGI_FORMAT_B8G8R8A8_UNORM) return 32;
		else if (dxgiFormat == DXGI_FORMAT_B8G8R8X8_UNORM) return 32;
		else if (dxgiFormat == DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM) return 32;

		else if (dxgiFormat == DXGI_FORMAT_R10G10B10A2_UNORM) return 32;
		else if (dxgiFormat == DXGI_FORMAT_B5G5R5A1_UNORM) return 16;
		else if (dxgiFormat == DXGI_FORMAT_B5G6R5_UNORM) return 16;
		else if (dxgiFormat == DXGI_FORMAT_R32_FLOAT) return 32;
		else if (dxgiFormat == DXGI_FORMAT_R16_FLOAT) return 16;
		else if (dxgiFormat == DXGI_FORMAT_R16_UNORM) return 16;
		else if (dxgiFormat == DXGI_FORMAT_R8_UNORM) return 8;
		else if (dxgiFormat == DXGI_FORMAT_A8_UNORM) return 8;

		return 0;
	}

	void SetViewMatrix(
		DirectX::XMFLOAT4X4& rViewMatrix,
		const float x,
		const float y,
		const float z)
	{
		DirectX::XMStoreFloat4x4(&rViewMatrix,
			DirectX::XMMatrixLookToLH(
				{ x, y, z },
				{ 0.0f, 0.0f,  1.0f },
				{ 0.0f, 1.0f,  0.0f }
		));
	}

	void SetProjectionMatrix(
		DirectX::XMFLOAT4X4& rProjectionMatrix,
		const float fov,
		const float aspectRatio,
		const float nearPlane,
		const float farPlane)
	{
		DirectX::XMStoreFloat4x4(&rProjectionMatrix,
			DirectX::XMMatrixPerspectiveFovLH(
				fov, aspectRatio, nearPlane, farPlane
			));
	}

	void SetProjectionMatrixInverse(
		DirectX::XMFLOAT4X4& rProjectionMatrix,
		const float fov,
		const float aspectRatio,
		const float nearPlane,
		const float farPlane)
	{
		DirectX::XMMATRIX projection = DirectX::XMMatrixPerspectiveFovLH(
			fov, aspectRatio, nearPlane, farPlane);

		DirectX::XMVECTOR determinant = DirectX::XMMatrixDeterminant(projection);

		DirectX::XMStoreFloat4x4(&rProjectionMatrix,
			DirectX::XMMatrixInverse(&determinant, projection));
	}

	ForwardRenderingPipeline::ForwardRenderingPipeline()
	{
	}

	ForwardRenderingPipeline::~ForwardRenderingPipeline()
	{
	}

	HRESULT ForwardRenderingPipeline::Initialize(
		ID3D11Device4* pDevice4,
		const void* pDescription)
	{
		FORWARD_RENDERING_PIPELINE_DESC* pDesc = (FORWARD_RENDERING_PIPELINE_DESC*)pDescription;

		mClientWidth = pDesc->ClientWidth;
		mClientHeight = pDesc->ClientHeight;

		{
			D3D11_TEXTURE2D_DESC texture_desc = { 0 };
			texture_desc.Width = mClientWidth;
			texture_desc.Height = mClientHeight;
			texture_desc.ArraySize = 1;
			texture_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
			texture_desc.CPUAccessFlags = 0;
			texture_desc.Format = DXGI_FORMAT_R32_TYPELESS;
			texture_desc.SampleDesc = { 1, 0 };
			texture_desc.Usage = D3D11_USAGE_DEFAULT;

			pDevice4->CreateTexture2D(&texture_desc, NULL, &mpDepthTexture);

			D3D11_DEPTH_STENCIL_VIEW_DESC depth_desc = {};
			depth_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			depth_desc.Format = DXGI_FORMAT_D32_FLOAT;
			depth_desc.Texture2D.MipSlice = 0;

			pDevice4->CreateDepthStencilView(mpDepthTexture, &depth_desc, &mpDepthBuffer);
		
			/*D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
			srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srv_desc.Format = DXGI_FORMAT_R32_FLOAT;
			srv_desc.Texture2D.MipLevels = 1;
			srv_desc.Texture2D.MostDetailedMip = 0;

			pDevice4->CreateShaderResourceView(mpDepthTexture, &srv_desc, &mpDepthResource);*/


		}
		// Create depth stencil state
		{
			D3D11_DEPTH_STENCIL_DESC dsDesc;

			// Depth test parameters
			dsDesc.DepthEnable = true;
			dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;  

			// Stencil test parameters
			dsDesc.StencilEnable = false;
			//dsDesc.StencilReadMask = 0xFF;
			//dsDesc.StencilWriteMask = 0xFF;

			//// Stencil operations if pixel is front-facing
			//dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			//dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_REPLACE;
			//// Write to the stencil on pass
			//dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
			//dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

			//// Stencil operations if pixel is back-facing
			//dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			//dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
			//dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			//dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

			// Create depth stencil state
			pDevice4->CreateDepthStencilState(&dsDesc, &mpDepthStencilState);


		}

		{
			D3D11_BUFFER_DESC desc = { 0 };
			desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			desc.ByteWidth = sizeof(DirectX::XMFLOAT4X4);
			desc.CPUAccessFlags = 0;
			desc.Usage = D3D11_USAGE_DEFAULT;

			{
				DirectX::XMFLOAT4X4 proj_matrix;
				SetProjectionMatrix(
					proj_matrix,
					pDesc->Fov,
					pDesc->ClientWidth / (float)pDesc->ClientHeight,
					pDesc->NearPlane,
					pDesc->FarPlane);

				D3D11_SUBRESOURCE_DATA data = { 0 };
				data.pSysMem = &proj_matrix;

				pDevice4->CreateBuffer(&desc, &data, &mpMatrixBuffers[1]);
			}

			{
				DirectX::XMFLOAT4X4 proj_matrix;
				SetProjectionMatrixInverse(
					proj_matrix,
					pDesc->Fov,
					pDesc->ClientWidth / (float)pDesc->ClientHeight,
					pDesc->NearPlane,
					pDesc->FarPlane);

				D3D11_SUBRESOURCE_DATA data = { 0 };
				data.pSysMem = &proj_matrix;

				pDevice4->CreateBuffer(&desc, &data, &mpMatrixBuffers[2]);
			}
		}

		{
			D3D11_BUFFER_DESC desc = { 0 };
			desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			desc.ByteWidth = sizeof(DirectX::XMFLOAT4X4);
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			desc.Usage = D3D11_USAGE_DYNAMIC;

			pDevice4->CreateBuffer(&desc, NULL, &mpMatrixBuffers[0]);
		}

		//// Color Render Target
		//{
		//	ID3D11Texture2D* pTexture = NULL;
		//	{
		//		D3D11_TEXTURE2D_DESC desc = { 0 };
		//		desc.Width = pDesc->ClientWidth;
		//		desc.Height = pDesc->ClientHeight;
		//		desc.ArraySize = 1;
		//		desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		//		desc.CPUAccessFlags = 0;
		//		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		//		desc.MipLevels = 1;
		//		desc.MiscFlags = 0;
		//		desc.SampleDesc = { 1, 0 };
		//		desc.Usage = D3D11_USAGE_DEFAULT;

		//		pDevice4->CreateTexture2D(&desc, NULL, &pTexture);
		//	}
		//	{
		//		D3D11_RENDER_TARGET_VIEW_DESC desc = {};
		//		desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		//		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		//		desc.Texture2D.MipSlice = 0;

		//		pDevice4->CreateRenderTargetView(pTexture, &desc, &mpRenderTarget);
		//	}
		//	{
		//		D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
		//		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		//		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		//		desc.Texture2D.MipLevels = 1;
		//		desc.Texture2D.MostDetailedMip = 0;

		//		pDevice4->CreateShaderResourceView(
		//			pTexture,
		//			&desc,
		//			&mpShaderResource);
		//	}
		//	pTexture->Release();
		//}


		internal::GetBackBuffer(&mpRenderTarget);

		{
			D3D11_SAMPLER_DESC desc = {};
			desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
			desc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
			desc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
			desc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;

			pDevice4->CreateSamplerState(&desc, &mpSamplerState);
		}

		{
			IWICImagingFactory* p_image_factory = NULL;
			IWICBitmapDecoder* p_decoder = NULL;
			IWICBitmapFrameDecode* p_decoder_frame = NULL;

			CoInitializeEx(NULL, COINIT_MULTITHREADED);

			HRESULT hr;
			hr = CoCreateInstance(
				CLSID_WICImagingFactory,
				NULL,
				CLSCTX_INPROC_SERVER,
				IID_PPV_ARGS(&p_image_factory));

			if (FAILED(hr)) return E_ABORT;

			hr = p_image_factory->CreateDecoderFromFilename(
				L"../textures/random_normals_texture.jpg",
				NULL,
				GENERIC_READ,
				WICDecodeMetadataCacheOnLoad,
				&p_decoder);

			if (FAILED(hr)) return E_ABORT;

			p_decoder->GetFrame(0, &p_decoder_frame);

			WICPixelFormatGUID pixel_format;
			UINT image_width = 0, image_height = 0;
			p_decoder_frame->GetSize(&image_width, &image_height);
			p_decoder_frame->GetPixelFormat(&pixel_format);


			WICPixelFormatGUID pixel_format_converted = GetConvertToWICFormat(pixel_format);

			IWICFormatConverter* p_converter;
			p_image_factory->CreateFormatConverter(&p_converter);
			int can_convert = FALSE;
			p_converter->CanConvert(pixel_format, pixel_format_converted, &can_convert);
			if (!can_convert) return E_ABORT;

			p_converter->Initialize(
				p_decoder_frame,
				pixel_format_converted,
				WICBitmapDitherTypeErrorDiffusion,
				0,
				0,
				WICBitmapPaletteTypeCustom);

			DXGI_FORMAT format = GetDXGIFormatFromWICFormat(pixel_format_converted);
			const UINT pixel_data_pitch = image_width * GetDXGIFormatBitsPerPixel(format) / 8;
			const UINT pixel_data_bytewidth = pixel_data_pitch * image_height;

			BYTE* p_pixel_data = (BYTE*)malloc(pixel_data_bytewidth);

			hr = p_converter->CopyPixels(
				NULL,
				pixel_data_pitch,
				pixel_data_bytewidth,
				p_pixel_data);

			if (FAILED(hr)) return E_ABORT;

			{
				ID3D11Texture2D* pTexture = NULL;
				{
					D3D11_TEXTURE2D_DESC desc = { 0 };
					desc.Width = image_width;
					desc.Height = image_height;
					desc.ArraySize = 1;
					desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
					desc.CPUAccessFlags = 0;
					desc.Format = format;
					desc.MipLevels = 1;
					desc.MiscFlags = 0;
					desc.SampleDesc = { 1, 0 };
					desc.Usage = D3D11_USAGE_DEFAULT;

					D3D11_SUBRESOURCE_DATA data = { 0 };
					data.pSysMem = p_pixel_data;
					data.SysMemPitch = pixel_data_pitch;
					data.SysMemSlicePitch = pixel_data_bytewidth;

					hr = pDevice4->CreateTexture2D(&desc, &data, &pTexture);

					if (FAILED(hr)) return hr;
				}

				{
					D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
					desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
					desc.Format = format;
					desc.Texture2D.MipLevels = 1;
					desc.Texture2D.MostDetailedMip = 0;

					pDevice4->CreateShaderResourceView(
						pTexture,
						&desc,
						&mpRandomNormals);
				}

				pTexture->Release();
			}

			p_converter->Release();
			p_decoder_frame->Release();
			p_decoder->Release();
			p_image_factory->Release();

			free(p_pixel_data);
		}

		mClearColor[0] = pDesc->ClearColor[0];
		mClearColor[1] = pDesc->ClearColor[1];
		mClearColor[2] = pDesc->ClearColor[2];

		pDesc->pDepthBuffer = mpDepthBuffer;
		pDesc->pViewMatrixBuffer = mpMatrixBuffers[0];
		pDesc->pProjMatrixBuffer = mpMatrixBuffers[1];
		pDesc->pInvProjMatrixBuffer = mpMatrixBuffers[2];

		return S_OK;
	}

	void ForwardRenderingPipeline::Clear(ID3D11DeviceContext4* pContext4)
	{
		float color_clear[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		pContext4->ClearRenderTargetView(mpRenderTarget, mClearColor);

		pContext4->ClearDepthStencilView(mpDepthBuffer, D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	void ForwardRenderingPipeline::Update(ID3D11DeviceContext4* pContext4, const void* pPipelineData)
	{
		FORWARD_RENDERING_PIPELINE_DATA* pData = (FORWARD_RENDERING_PIPELINE_DATA*)pPipelineData;

		graphics::UploadToDynamicBuffer(
			pContext4,
			mpMatrixBuffers[0],
			D3D11_MAP_WRITE_DISCARD,
			&pData->ViewMatrix,
			sizeof(pData->ViewMatrix),
			0);
	}

	void ForwardRenderingPipeline::Begin(ID3D11DeviceContext4* pContext4)
	{
		ID3D11ShaderResourceView* pNULLView[2] = { NULL };
		pContext4->PSSetShaderResources(1, 2, pNULLView);

		pContext4->PSSetShaderResources(4, 1, &mpRandomNormals);

		graphics::SetViewport(pContext4, 0, 0, mClientWidth, mClientHeight);  

		pContext4->OMSetRenderTargets(1, &mpRenderTarget, mpDepthBuffer);

		pContext4->VSSetConstantBuffers(1, 2, mpMatrixBuffers);
		pContext4->OMSetDepthStencilState(this->mpDepthStencilState, 0);

		pContext4->PSSetSamplers(1, 1, &mpSamplerState);
	}

	void ForwardRenderingPipeline::PreProcess(
		ID3D11DeviceContext4* pContext4,
		ID3D11VertexShader* pVertexShader,
		ID3D11PixelShader* pPixelShader)
	{
		pContext4->VSSetShader(pVertexShader, NULL, 0);
		pContext4->PSSetShader(pPixelShader, NULL, 0);
	}

	void ForwardRenderingPipeline::End(ID3D11DeviceContext4* pContext4)
	{
		//ID3D11RenderTargetView* pNULLView[2] = { NULL };
		//pContext4->OMSetRenderTargets(2, pNULLView, NULL);

		//pContext4->PSSetConstantBuffers(0, 1, &mpMatrixBuffers[2]);

		//pContext4->PSSetShaderResources(1, 1, &mpShaderResource);
	}

	void ForwardRenderingPipeline::Destroy()
	{
		mpDepthBuffer->Release();
		mpDepthTexture->Release();

		mpRenderTarget->Release();

		//mpShaderResource->Release();

		mpMatrixBuffers[0]->Release();
		mpMatrixBuffers[1]->Release();
		mpMatrixBuffers[2]->Release();

		mpSamplerState->Release();
	}
}