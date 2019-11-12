#include "SSAOPipeline.h"

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

	SSAOPipeline::SSAOPipeline()
	{
	}

	SSAOPipeline::~SSAOPipeline()
	{
	}

	HRESULT SSAOPipeline::Initialize(
		ID3D11Device4* pDevice4,
		const void* pDescription)
	{
		HRESULT hr = S_OK;
		SSAO_PIPELINE_DESC* pDesc = (SSAO_PIPELINE_DESC*)pDescription;

		mWidth		= pDesc->Width;
		mHeight	= pDesc->Height;

		// SSAO Render Target
		{
			ID3D11Texture2D* pTexture = NULL;
			{
				D3D11_TEXTURE2D_DESC desc = { 0 };
				desc.Width = mWidth;
				desc.Height = mHeight;
				desc.ArraySize = 1;
				desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
				desc.CPUAccessFlags = 0;
				desc.Format = DXGI_FORMAT_R8_UNORM;
				desc.MipLevels = 0;
				desc.MiscFlags = 0;
				desc.SampleDesc = { 1, 0 };
				desc.Usage = D3D11_USAGE_DEFAULT;

				pDevice4->CreateTexture2D(&desc, NULL, &pTexture);
			}
			{
				D3D11_RENDER_TARGET_VIEW_DESC desc = {};
				desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
				desc.Format = DXGI_FORMAT_R8_UNORM;
				desc.Texture2D.MipSlice = 0;

				pDevice4->CreateRenderTargetView(pTexture, &desc, &mpSSAOTarget);
			}
			{
				D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
				desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				desc.Format = DXGI_FORMAT_R8_UNORM;
				desc.Texture2D.MipLevels = 1;
				desc.Texture2D.MostDetailedMip = 0;

				pDevice4->CreateShaderResourceView(
					pTexture,
					&desc,
					&mpSSAOResource);
			}

			pTexture->Release();
		}

		// SSAO Random Normals

		IWICImagingFactory* p_image_factory = NULL;
		IWICBitmapDecoder* p_decoder = NULL;
		IWICBitmapFrameDecode* p_decoder_frame = NULL;

		CoInitializeEx(NULL, COINIT_MULTITHREADED);

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

		return S_OK;
	}

	void SSAOPipeline::Update(ID3D11DeviceContext4* pContext4, const void* pPipelineData)
	{
		SSAO_PIPELINE_DATA* pData = (SSAO_PIPELINE_DATA*)pPipelineData;
	}

	void SSAOPipeline::Begin(ID3D11DeviceContext4* pContext4)
	{
		/*ID3D11ShaderResourceView* pNull = { NULL };
		pContext4->PSSetShaderResources(3, 1, &pNull);*/

		graphics::SetViewport(pContext4, 0, 0, mWidth, mHeight);
		pContext4->OMSetRenderTargets(1, &mpSSAOTarget, NULL);

		pContext4->PSSetShaderResources(4, 1, &mpRandomNormals);
	}

	void SSAOPipeline::PreProcess(
		ID3D11DeviceContext4* pContext4,
		ID3D11VertexShader* pVertexShader,
		ID3D11PixelShader* pPixelShader)
	{
		pContext4->VSSetShader(pVertexShader, NULL, 0);
		pContext4->PSSetShader(pPixelShader, NULL, 0);
	}

	void SSAOPipeline::End(ID3D11DeviceContext4* pContext4)
	{
		ID3D11RenderTargetView* pNull = { NULL };
		pContext4->OMSetRenderTargets(1, &pNull, NULL);

		pContext4->PSSetShaderResources(3, 1, &mpSSAOResource);
	}

	void SSAOPipeline::Destroy()
	{
		mpSSAOResource->Release();
		mpSSAOTarget->Release();

		mpRandomNormals->Release();
	}
}