#include "Direct2D.h"

Direct2D::Direct2D()
{

	this->mpHwndRenderTarget = nullptr;
	this->mpWicFactory = nullptr;
	this->mpFormatConverter = nullptr;
	this->mpDecoder = nullptr;
	this->mpBitmapSrc = nullptr;
	this->mpTextFactory = nullptr;
	this->mpFactory = nullptr;
	this->mpDevice = nullptr;
	this->mpContext = nullptr;
	this->mColorText = nullptr;
	this->bitmap = nullptr;
	this->mTrimmer = {};
	this->mTrimmer.granularity = DWRITE_TRIMMING_GRANULARITY_CHARACTER;
	this->mfont = L"Arial";
	this->mfontSize = 40;
	this->mInit();
}

Direct2D::~Direct2D()
{
	if (this->mpHwndRenderTarget != nullptr)
		this->mpHwndRenderTarget->Release();
	if (this->mpTextFactory != nullptr)
		this->mpTextFactory->Release();
	if (this->mpFactory != nullptr)
		this->mpFactory->Release();
	if(this->mpTextFormat != nullptr)
		this->mpTextFormat->Release();
	//this->mpDevice->Release();
	//this->mpContext->Release();
	if (this->mColorText != nullptr)
		this->mColorText->Release();
	if(this->mpWicFactory != nullptr)
		this->mpWicFactory->Release();
	if(this->mpFormatConverter != nullptr)
		this->mpFormatConverter->Release();
	if(this->mpDecoder != nullptr)
		this->mpDecoder->Release();
	if(this->mpBitmapSrc != nullptr)
		this->mpBitmapSrc->Release();
}

void Direct2D::CreateHwndRenderTarget(HWND window, RECT* rect) //Creates a render target that is a window,needs a window handle and how large you want the rendertarget
{
	HRESULT hr = this->mpFactory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(
			window,
			D2D1::SizeU(rect->right - rect->left, rect->bottom - rect->top)),
		&this->mpHwndRenderTarget);
	this->mpHwndRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
	this->mpHwndRenderTarget->AddRef(); //add reference counter
	this->mCreateColorText();
	this->mCreateColorDraw();
	this->mCreateTextFormat();

	this->mpTextFormat->AddRef();
	this->mColorText->AddRef();
	this->mColorDraw->AddRef();
	this->mpRect = rect;
}

void Direct2D::InitDeviceAndContext(IDXGIDevice* dxgiDevice) //takes DXGIdevice from Dx11
{
	this->mpFactory->CreateDevice(dxgiDevice, &this->mpDevice);
	this->mpDevice->CreateDeviceContext(
		D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
		&this->mpContext);
}

ID2D1DeviceContext* Direct2D::GetpContext()
{
	return this->mpContext;
}

void Direct2D::LoadImageToBitmap(std::string imageFilePath, D2D1_RECT_F drawRect) //loads an image to a bitmap struct vector
{
	BitmapInfo new_bitmap_struct;
	new_bitmap_struct.name = imageFilePath;
	new_bitmap_struct.drawArea = drawRect;
	std::wstring w_str = this->mStrToWstrConverter(imageFilePath);

	if (SUCCEEDED(this->mpWicFactory->CreateFormatConverter(&this->mpFormatConverter)))
	{
		if (SUCCEEDED(this->mpWicFactory->CreateDecoderFromFilename(w_str.c_str(), NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &this->mpDecoder)))
		{
			if (SUCCEEDED(this->mpDecoder->GetFrame(0, &this->mpBitmapSrc)))
			{
				if (SUCCEEDED(this->mpFormatConverter->Initialize(this->mpBitmapSrc, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 1.f, WICBitmapPaletteTypeMedianCut)))
				{
					if (SUCCEEDED(this->mpHwndRenderTarget->CreateBitmapFromWicBitmap(this->mpFormatConverter, &new_bitmap_struct.bitmap)))
					{
						this->mBitMapLoaded = true;
					}
				}
			}
		}
	};

	this->mBitmapVector.push_back(new_bitmap_struct);
}

ID2D1Bitmap* Direct2D::GetBitmapByName(std::string bitmapName)
{
	ID2D1Bitmap* to_return = nullptr; // Default return is nullptr
	std::vector<BitmapInfo>::iterator it;

	for (it = this->mBitmapVector.begin();
		it != this->mBitmapVector.end();
		++it)
	{
		if (bitmapName.compare((*it).name) == 0) // Bitmap with correct name found
		{
			to_return = (*it).bitmap; // Return pointer to button
			it = this->mBitmapVector.end() - 1; // Set iterator to end
												// -1 because incrementation is performed after this.
												// Incrementing on .end() is a baaad idea.
		}
	}
	return to_return;
}

void Direct2D::DrawBitmap() //iterates through the bitmap vector and draws everything, if specific bitmap needs to be drawn an other draw function needs to be added
{
	std::vector<BitmapInfo>::iterator it;
	for (it = this->mBitmapVector.begin();
		it != this->mBitmapVector.end();
		++it)
	{
		
		this->mpHwndRenderTarget->DrawBitmap((*it).bitmap, (*it).drawArea, 1, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, D2D1::RectF(0, 0, (*it).bitmap->GetSize().width, (*it).bitmap->GetSize().height));
		
	}
}

void Direct2D::PrintText(std::string text, RECT rect) //draws text with format, draw area and color
{
	std::wstring w_str = this->mStrToWstrConverter(text); //Converts string to widestring

	this->mCreateTextFormat();
	D2D1_RECT_F layoutRect = D2D1::RectF(rect.left, rect.top, rect.right, rect.bottom);
	this->mpHwndRenderTarget->DrawTextA(w_str.c_str(), wcslen(w_str.c_str()), this->mpTextFormat, layoutRect, this->mColorText);


}

void Direct2D::PrintText(std::string test, int left, int top, int right, int bottom)//draws text with format, draw area and color
{
	std::wstring w_str = this->mStrToWstrConverter(test);

	
	this->mpTextFormat->AddRef();
	D2D1_RECT_F layoutRect = D2D1::RectF(left, top, right, bottom);
	this->mpHwndRenderTarget->DrawTextA(w_str.c_str(), wcslen(w_str.c_str()), this->mpTextFormat, layoutRect, this->mColorText);

}

void Direct2D::setTextColor(float r, float g, float b, float a)
{
	this->mColorText->SetColor(D2D1::ColorF(r, g, b, a));
}

void Direct2D::setDrawColor(float r, float g, float b, float a)
{
	this->mColorDraw->SetColor(D2D1::ColorF(r, g, b, a));
}

void Direct2D::setTextSize(unsigned int size)//updates the text format
{
	HRESULT hr = this->mpTextFactory->CreateTextFormat(
		this->mfont.c_str(),
		NULL,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		size,
		L"en-US",
		&this->mpTextFormat
	);
	this->mpTextFormat->SetTrimming(&this->mTrimmer, NULL);
	this->mpTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_CHARACTER);
	this->mpTextFormat->AddRef();
}

void Direct2D::setFont(std::string font)//updates the text format
{
	std::wstring w_font = this->mStrToWstrConverter(font);
	this->mfont = w_font;
	HRESULT hr = this->mpTextFactory->CreateTextFormat(
		this->mfont.c_str(),
		NULL,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		this->mfontSize,
		L"en-US",
		&this->mpTextFormat
	);
	this->mpTextFormat->SetTrimming(&this->mTrimmer, NULL);
	this->mpTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_CHARACTER);
	this->mpTextFormat->AddRef();
}

void Direct2D::drawRect(RECT rect, int thickness) //draws a rect with line thickness and color
{
	this->mpHwndRenderTarget->DrawRectangle(
		D2D1::RectF(rect.left, rect.top, rect.right, rect.bottom),
		this->mColorDraw,
		thickness);

}

void Direct2D::solidRect(RECT rect) //draws a filled rect with color
{

	this->mpHwndRenderTarget->FillRectangle(
		D2D1::RectF(rect.left, rect.top, rect.right, rect.bottom),
		this->mColorDraw);
}

ID2D1HwndRenderTarget* Direct2D::getHwndRenderTarget()
{
	return this->mpHwndRenderTarget;
}

void Direct2D::mCreateFactory()
{
	HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &this->mpFactory); //used for creating all basic things like device and rendertarget
}

void Direct2D::mCreateWicFactory()
{
	CoInitialize(nullptr);
	HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&this->mpWicFactory));
}

void Direct2D::mCreateTextFactory()
{
	HRESULT hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory7), reinterpret_cast<IUnknown * *>(&this->mpTextFactory));
}

void Direct2D::mCreateTextFormat()
{
	HRESULT hr = this->mpTextFactory->CreateTextFormat(
		L"Comic_sans",
		NULL,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		50,
		L"en-US",
		&this->mpTextFormat
	);
	this->mpTextFormat->SetTrimming(&this->mTrimmer, NULL);
	this->mpTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_CHARACTER);
	this->mpTextFormat->AddRef();
}

void Direct2D::mCreateColorText()
{
	HRESULT hr = this->mpHwndRenderTarget->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::DarkGoldenrod),
		&this->mColorText
	);
}

void Direct2D::mCreateColorDraw()
{
	HRESULT hr = this->mpHwndRenderTarget->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::DarkGoldenrod),
		&this->mColorDraw
	);
}

std::wstring Direct2D::mStrToWstrConverter(std::string str) //Converts string to widestring
{
	std::wstring_convert<std::codecvt<wchar_t, char, std::mbstate_t>> convert;
	std::wstring w_str = convert.from_bytes(str);

	return w_str;
}

void Direct2D::mInit() //Creates factories for d2d and dwrite which are foundations for other functionalities
{
	this->mCreateFactory();
	this->mCreateTextFactory();
	this->mCreateWicFactory();
}
