#include "Direct2D.h"

Direct2D::Direct2D()
{

	this->mpDevice = nullptr;
	this->mpDecoder = nullptr;
	this->mpFactory = nullptr;
	this->mpContext = nullptr;
	this->mpColorText = nullptr;
	this->mpColorDraw = nullptr;
	this->mpBitmapSrc = nullptr;
	this->mpFailBitmap = nullptr;
	this->mpWicFactory = nullptr;
	this->mpTextFactory = nullptr;
	//this->mpTextFormat = nullptr; not in use right now but dont want to remove because it will be harder to fix if its needed
	this->mpFormatConverter = nullptr;
	this->mpDebugTextFormat = nullptr;
	this->mpBackbufferBitmap = nullptr;
	this->mpHwndRenderTarget = nullptr;
	this->mTrimmer.granularity = DWRITE_TRIMMING_GRANULARITY_CHARACTER;
	this->mTrimmer = {};
	this->mWidth = 100;
	this->mHeight = 100;
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

	//if (this->mpTextFormat != nullptr)
	//	this->mpTextFormat->Release();

	if (this->mpDebugTextFormat != nullptr)
		this->mpDebugTextFormat->Release();

	if (this->mpDevice != nullptr)
		this->mpDevice->Release();

	if (this->mpContext != nullptr)
		this->mpContext->Release();

	if (this->mpColorText != nullptr)
		this->mpColorText->Release();

	if (this->mpColorDraw != nullptr)
		this->mpColorDraw->Release();

	if (this->mpWicFactory != nullptr)
		this->mpWicFactory->Release();

	if (this->mpFormatConverter != nullptr)
		this->mpFormatConverter->Release();

	if (this->mpBackbufferBitmap != nullptr)
		this->mpBackbufferBitmap->Release();

	if (this->mpDecoder != nullptr)
		this->mpDecoder->Release();

	if (this->mpBitmapSrc != nullptr)
		this->mpBitmapSrc->Release();

	if (this->mpFailBitmap != nullptr)
		this->mpFailBitmap->Release();

	for (BitmapPair pair : this->mBitmapList)
	{
		pair.second->Release();
	}
	for (BrushMapPair pair : this->mBrushMap)
	{
		pair.second->Release();
	}
	for (BitmapPairChar pair : this->mBitmapListChar)
	{
		pair.second->Release();
	}
	for (int i = 0; i < COLOR_BRUSHES; i++)
	{
		this->mColorBrushes[i]->Release();
	}
	for (int i = 0; i < NR_OF_FORMATS; i++)
	{
		this->mpTextFormats[i]->Release();
	}
}

HRESULT Direct2D::CreateHwndRenderTarget(HWND window, RECT* rect) //Creates a render target that is a window,needs a window handle and how large you want the rendertarget
{
	HRESULT hr = E_FAIL;
	if (SUCCEEDED(hr = this->mpFactory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(
			window,
			D2D1::SizeU(rect->right - rect->left, rect->bottom - rect->top)),
		&this->mpHwndRenderTarget)))
	{
		this->mHwndRenderTargetCreated = true;
		this->mpHwndRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
		this->mpHwndRenderTarget->AddRef(); //add reference counter
		hr = this->mCreateColorText();
		hr = this->mCreateColorDraw();
		hr = this->mCreateTextFormat(L"Times New Roman",20,&this->mpDebugTextFormat);//textformat for debug
		this->mpDebugTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_CHARACTER);
		hr = this->mCreateColorBrushes();
		this->LoadImageToBitmap("fail.png");
		this->mpColorText->AddRef();
		this->mpColorDraw->AddRef();
		this->mpRect = rect;
		this->mWidth = rect->right;
		this->mHeight = rect->bottom;
	};
	
	return hr;
}

HRESULT Direct2D::CreateHwndRenderTarget(HWND window, int width, int height)
{
	HRESULT hr = E_FAIL;
	if (SUCCEEDED(hr = this->mpFactory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(
			window,
			D2D1::SizeU(width, height)),
		&this->mpHwndRenderTarget)))
	{
		this->mHwndRenderTargetCreated = true;
		this->mpHwndRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
		this->mpHwndRenderTarget->AddRef(); //add reference counter
		hr = this->mCreateColorText();
		hr = this->mCreateColorDraw();
		hr = this->mCreateTextFormat(L"Times New Roman", 20, &this->mpDebugTextFormat);//textformat for debug
		this->mpDebugTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_CHARACTER);
		hr = this->mCreateColorBrushes();
		this->LoadImageToBitmap("fail.png");
		this->mpColorText->AddRef();
		this->mpColorDraw->AddRef();
		this->mWidth = width;
		this->mHeight = height;
	};

	return hr;
}

void Direct2D::InitDeviceAndContext(IDXGIDevice* dxgiDevice) //takes DXGIdevice from Dx11
{
	HRESULT hr = E_FAIL;
	if (SUCCEEDED(this->mpFactory->CreateDevice(dxgiDevice, &this->mpDevice)));
		if (SUCCEEDED(this->mpDevice->CreateDeviceContext(
			D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
			&this->mpContext)));
		{
			this->mpContext->SetTarget(this->mpBackbufferBitmap);
			this->mDeviceContextCreated = true;
			hr = this->mCreateColorText();
			hr = this->mCreateColorDraw();
			hr = this->mCreateTextFormat(L"Times New Roman", 20, &this->mpDebugTextFormat);//textformat for debug
			this->mpDebugTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_CHARACTER);
			hr = this->mCreateColorBrushes();
			this->mCreateTextFormats();
			this->LoadImageToBitmap("../../UI/Resource/fail.png");
		}
}

ID2D1DeviceContext* Direct2D::GetpContext()
{
	return this->mpContext;
}

ID2D1Bitmap1* Direct2D::LoadImageToBitmap(std::string imageFilePath, char bitmapName[BITMAP_NAME_LENGTH]/*, D2D1_RECT_F drawRect*/) //loads an image to a bitmap map with bitmap and ID
{
	HRESULT hr = E_FAIL;
	ID2D1Bitmap1* new_bitmap = nullptr;
	std::wstring w_str = this->mStrToWstrConverter(imageFilePath);
	if (this->mDeviceContextCreated)
	{
		if (SUCCEEDED(hr = this->mpWicFactory->CreateFormatConverter(&this->mpFormatConverter)))
		{
			if (SUCCEEDED(hr = this->mpWicFactory->CreateDecoderFromFilename(w_str.c_str(), NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &this->mpDecoder)))
			{
				if (SUCCEEDED(hr = this->mpDecoder->GetFrame(0, &this->mpBitmapSrc)))
				{
					if (SUCCEEDED(hr = this->mpFormatConverter->Initialize(this->mpBitmapSrc, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 1.f, WICBitmapPaletteTypeMedianCut)))
					{
						if (SUCCEEDED(hr = this->mpContext->CreateBitmapFromWicBitmap(this->mpFormatConverter, &new_bitmap)))
						{
							this->mBitmapListChar[bitmapName] = new_bitmap; //use this if someone forget to "fetch" the bitmap when this function is called or else you lose the pointer
							return new_bitmap;
						}
					}
				}
			}
		}
	}
	return nullptr;
}

ID2D1Bitmap1* Direct2D::LoadImageToBitmap(std::string imageFilePath, std::string bitmapName)
{
	HRESULT hr = E_FAIL;
	ID2D1Bitmap1* new_bitmap = nullptr;
	std::wstring w_str = this->mStrToWstrConverter(imageFilePath);
	if (this->mDeviceContextCreated)
	{
		if (SUCCEEDED(hr = this->mpWicFactory->CreateFormatConverter(&this->mpFormatConverter)))
		{
			if (SUCCEEDED(hr = this->mpWicFactory->CreateDecoderFromFilename(w_str.c_str(), NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &this->mpDecoder)))
			{
				if (SUCCEEDED(hr = this->mpDecoder->GetFrame(0, &this->mpBitmapSrc)))
				{
					if (SUCCEEDED(hr = this->mpFormatConverter->Initialize(this->mpBitmapSrc, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 1.f, WICBitmapPaletteTypeMedianCut)))
					{
						if (SUCCEEDED(hr = this->mpContext->CreateBitmapFromWicBitmap(this->mpFormatConverter, &new_bitmap)))
						{
							this->mBitmapList[bitmapName] = new_bitmap; //use this if someone forget to "fetch" the bitmap when this function is called or else you lose the pointer
							return new_bitmap;
						}
					}
				}
			}
		}
	}
	return nullptr;
}

ID2D1Bitmap1* Direct2D::GetBitmap(char* bitmapName)
{
	ID2D1Bitmap1* to_return = nullptr;
	for (BitmapPairChar pair : this->mBitmapListChar)
	{
		if (this->CharArrayCompare(pair.first, bitmapName, BITMAP_NAME_LENGTH))
			return pair.second;
	}
	return to_return;
}

ID2D1Bitmap1* Direct2D::GetBitmap(std::string bitmapName)
{
	ID2D1Bitmap1* to_return = nullptr;
	for (BitmapPair pair : this->mBitmapList)
	{
		if (pair.first == bitmapName)
			return pair.second;
	}
	return to_return;
}

ID2D1Bitmap1* Direct2D::GetBackbufferBitmap()
{
	return this->mpBackbufferBitmap;
}

void Direct2D::SetBackbufferBitmap(ID2D1Bitmap1* backbuffer_bitmap)
{
	this->mpBackbufferBitmap = backbuffer_bitmap;
	this->mpContext->SetTarget(this->mpBackbufferBitmap);
	this->mWidth = this->mpBackbufferBitmap->GetSize().width;
	this->mHeight = this->mpBackbufferBitmap->GetSize().height;
}

ID2D1SolidColorBrush* Direct2D::GetBrushFromName(char* brushName)
{
	ID2D1SolidColorBrush* to_return = nullptr;
	for (BrushMapPair pair : this->mBrushMap)
	{
		if (this->CharArrayCompare(pair.first, brushName, BITMAP_NAME_LENGTH))
			return pair.second;
	}
	return to_return;
}

bool Direct2D::DrawBitmap(ID2D1Bitmap* bitmap, D2D1_RECT_F rect)
{
	bool canDraw = false;
	if (this->mDeviceContextCreated && bitmap != nullptr)
	{
		canDraw = true;
		this->mpContext->DrawBitmap(bitmap, rect, 1, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, D2D1::RectF(0, 0, bitmap->GetSize().width, bitmap->GetSize().height));
	}
	else
	{
		this->mpContext->DrawBitmap(this->mpFailBitmap, rect, 1, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, D2D1::RectF(0, 0, this->mpFailBitmap->GetSize().width, this->mpFailBitmap->GetSize().height));
	}
	return canDraw;
}

//void Direct2D::DrawBitmap() //iterates through the bitmap vector and draws everything, if specific bitmap needs to be drawn an other draw function needs to be added
//{
//	for (BitmapPair pair : this->mBitmapList)
//	{
//		
//		this->mpHwndRenderTarget->DrawBitmap(pair.second.bitmap, pair.second.drawArea, 1, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, D2D1::RectF(0, 0, pair.second.bitmap->GetSize().width, pair.second.bitmap->GetSize().height));
//		
//	}
//}

bool Direct2D::PrintText(std::string text, RECT rect) //draws text with format, draw area and color
{
	std::wstring w_str = this->mStrToWstrConverter(text); //Converts string to widestring

	D2D1_RECT_F layoutRect = D2D1::RectF(rect.left, rect.top, rect.right, rect.bottom);
	if (this->mDeviceContextCreated)
	{
		this->mpContext->DrawTextA(w_str.c_str(), wcslen(w_str.c_str()), this->mpTextFormats[2], layoutRect, this->mpColorText);
	}
	return this->mDeviceContextCreated;
}

bool Direct2D::PrintDebug(std::string text)
{
	std::wstring w_str = this->mStrToWstrConverter(text); //Converts string to widestring
	D2D1_RECT_F rect = D2D1::RectF(this->mWidth - (this->mWidth / 4), 0, this->mWidth, this->mHeight - (2*this->mHeight / 3)); // probably will need adjustment later on 
	
	if (this->mDeviceContextCreated)
	{
		//this->mpHwndRenderTarget->FillRectangle(rect, this->mpColorDraw);
		//this->mpHwndRenderTarget->DrawTextA(w_str.c_str(), wcslen(w_str.c_str()), this->mpDebugTextFormat, rect, this->mColorBrushes[Red]);
		this->mpContext->FillRectangle(rect, this->mpColorDraw);
		this->mpContext->DrawTextA(w_str.c_str(), wcslen(w_str.c_str()), this->mpDebugTextFormat, rect, this->mColorBrushes[Red]);
	}
	return this->mDeviceContextCreated;
}

bool Direct2D::PrintText(std::string text, D2D1_RECT_F rect, brushColors color, int size)
{
	std::wstring w_str = this->mStrToWstrConverter(text); //Converts string to widestring
	if (this->mDeviceContextCreated)
	{
		this->mpContext->DrawTextA(w_str.c_str(), wcslen(w_str.c_str()), this->mpTextFormats[size], rect, this->mColorBrushes[color]);
	}
	return this->mHwndRenderTargetCreated;
}

bool Direct2D::PrintText(std::string text, int left, int top, int right, int bottom)//draws text with format, draw area and color
{
	std::wstring w_str = this->mStrToWstrConverter(text);

	D2D1_RECT_F layoutRect = D2D1::RectF(left, top, right, bottom);
	if (this->mDeviceContextCreated)
	{
		this->mpContext->DrawTextA(w_str.c_str(), wcslen(w_str.c_str()), this->mpTextFormats[2], layoutRect, this->mpColorText);
	}
	return this->mDeviceContextCreated;
}

void Direct2D::SetTextColor(float r, float g, float b, float a)
{
	this->mpColorText->SetColor(D2D1::ColorF(r, g, b, a));
}

void Direct2D::SetDrawColor(float r, float g, float b, float a)
{
	this->mpColorDraw->SetColor(D2D1::ColorF(r, g, b, a));
}

void Direct2D::SetBrushColor(char* name, float r, float g, float b, float a)
{
	this->mBrushMap.at(name)->SetColor(D2D1::ColorF(r, g, b, a));
}

//HRESULT Direct2D::setTextSize(unsigned int size)//updates the text format //was used for a general text format dont know if it will be needed later
//{
//	HRESULT hr = this->mpTextFactory->CreateTextFormat(
//		this->mfont.c_str(),
//		NULL,
//		DWRITE_FONT_WEIGHT_NORMAL,
//		DWRITE_FONT_STYLE_NORMAL,
//		DWRITE_FONT_STRETCH_NORMAL,
//		size,
//		L"en-US",
//		&this->mpTextFormat
//	);
//	this->mpTextFormat->SetTrimming(&this->mTrimmer, NULL);
//	this->mpTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_CHARACTER);
//	this->mpTextFormat->AddRef();
//	return hr;
//}
//
//HRESULT Direct2D::setFont(std::string font)//updates the text format
//{
//	std::wstring w_font = this->mStrToWstrConverter(font);
//	this->mfont = w_font;
//	HRESULT hr = this->mpTextFactory->CreateTextFormat(
//		this->mfont.c_str(),
//		NULL,
//		DWRITE_FONT_WEIGHT_NORMAL,
//		DWRITE_FONT_STYLE_NORMAL,
//		DWRITE_FONT_STRETCH_NORMAL,
//		this->mfontSize,
//		L"en-US",
//		&this->mpTextFormat
//	);
//	this->mpTextFormat->SetTrimming(&this->mTrimmer, NULL);
//	this->mpTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_CHARACTER);
//	this->mpTextFormat->AddRef();
//	return hr;
//}

bool Direct2D::DrawRect(D2D1_RECT_F rect, int thickness, brushColors color) //draws a rect with line thickness and color
{
	if (this->mDeviceContextCreated)
	{
		this->mpContext->DrawRectangle(
			D2D1::RectF(rect.left, rect.top, rect.right, rect.bottom),
			this->mColorBrushes[color],
			thickness);
	}
	return this->mDeviceContextCreated;
}

bool Direct2D::SolidRect(D2D1_RECT_F rect, brushColors color) //draws a filled rect with color
{
	if (this->mDeviceContextCreated)
	{
		this->mpContext->FillRectangle(
			D2D1::RectF(rect.left, rect.top, rect.right, rect.bottom),
			this->mColorBrushes[color]);
	}
	return this->mDeviceContextCreated;
}

int Direct2D::CharArrayCompare(char* s1, char* s2, size_t sz) //A compares char arrays
{
	while (sz != 0) {
		// At end of both strings, equal.
		if ((*s1 == '\0') && (*s2 == '\0')) break;

		// Treat spaces at end and end-string the same.
		if ((*s1 == '\0') && (*s2 == ' ')) { s2++; sz--; continue; }
		if ((*s1 == ' ') && (*s2 == '\0')) { s1++; sz--; continue; }

		// Detect difference otherwise.
		if (*s1 != *s2) return 0;

		s1++; s2++; sz--;
	}
	return 1;
}

ID2D1HwndRenderTarget* Direct2D::GetHwndRenderTarget()
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
	HRESULT hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory7), reinterpret_cast<IUnknown**>(&this->mpTextFactory));
}

HRESULT Direct2D::mCreateTextFormat(std::wstring font, int size, IDWriteTextFormat** format)
{
	//std::wstring w_font = this->mStrToWstrConverter(font);
	HRESULT hr = this->mpTextFactory->CreateTextFormat(
		font.c_str(),
		NULL,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		size,
		L"en-US",
		format);
	(*format)->SetTrimming(&this->mTrimmer, NULL);
	(*format)->SetWordWrapping(DWRITE_WORD_WRAPPING_CHARACTER);
	(*format)->AddRef();
	return hr;
}

HRESULT Direct2D::mCreateColorText()
{
	HRESULT hr = E_FAIL;
	if (this->mDeviceContextCreated)
	{ 
		hr = this->mpContext->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::DarkGoldenrod),
		&this->mpColorText);
	}
	char name[10] = "text";
	this->mBrushMap[name] = this->mpColorText;
	return hr;
}

HRESULT Direct2D::mCreateColorDraw()
{
	HRESULT hr = E_FAIL;
	if (this->mDeviceContextCreated)
	{
		hr = this->mpContext->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Gray,0.5),
		&this->mpColorDraw);
	}
	char name[10] = "draw";
	this->mBrushMap[name] = this->mpColorDraw;
	return hr;
}

HRESULT Direct2D::mCreateColorBrushes()
{
	HRESULT hr = E_FAIL;
	if (this->mDeviceContextCreated)
	{
		hr = this->mpContext->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::Black),
			&this->mColorBrushes[0]);
		hr = this->mpContext->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::White), //Color does not have same enum as defined
			&this->mColorBrushes[1]);
		hr = this->mpContext->CreateSolidColorBrush(//red
			D2D1::ColorF(D2D1::ColorF(117.f/255.f, 1.f/255.f, 1.f/255.f, 1)),
			&this->mColorBrushes[2]);
		hr = this->mpContext->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF(47.f/255.f, 62.f/255.f, 236.f/255.f, 1)),//blue
			&this->mColorBrushes[3]);
		hr = this->mpContext->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF(0.f, 93.f/255.f, 5.f/255.f, 1)),//green
			&this->mColorBrushes[4]);
		hr = this->mpContext->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF(74.f/255.f, 1.f/255.f, 117.f/255.F, 1)),//purple
			&this->mColorBrushes[5]);
		hr = this->mpContext->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF(0.75f, 0.75f, 0.75f, 0.5f)),
			&this->mColorBrushes[6]);
		hr = this->mpContext->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF(0, 100, 100, 1)),
			&this->mColorBrushes[7]);
	}
	return hr;
}

HRESULT Direct2D::mCreateTextFormats()
{
	HRESULT hr = E_FAIL;
	if (this->mpTextFactory != nullptr)
	{
		this->mpTextFactory->CreateTextFormat(
			this->mfont.c_str(),
			NULL,
			DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			20,
			L"en-US",
			&this->mpTextFormats[0]);

		this->mpTextFactory->CreateTextFormat(
			this->mfont.c_str(),
			NULL,
			DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			30,
			L"en-US",
			&this->mpTextFormats[1]);

		this->mpTextFactory->CreateTextFormat(
			this->mfont.c_str(),
			NULL,
			DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			40,
			L"en-US",
			&this->mpTextFormats[2]);
	}
	return hr;
}

HRESULT Direct2D::LoadImageToBitmap(std::string imageFilePath)
{
	HRESULT hr = E_FAIL;
	
	std::wstring w_str = this->mStrToWstrConverter(imageFilePath);
	if (this->mDeviceContextCreated)
	{
		if (SUCCEEDED(hr = this->mpWicFactory->CreateFormatConverter(&this->mpFormatConverter)))
		{
			if (SUCCEEDED(hr = this->mpWicFactory->CreateDecoderFromFilename(w_str.c_str(), NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &this->mpDecoder)))
			{
				if (SUCCEEDED(hr = this->mpDecoder->GetFrame(0, &this->mpBitmapSrc)))
				{
					if (SUCCEEDED(hr = this->mpFormatConverter->Initialize(this->mpBitmapSrc, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 1.f, WICBitmapPaletteTypeMedianCut)))
					{
						if (SUCCEEDED(hr = this->mpContext->CreateBitmapFromWicBitmap(this->mpFormatConverter, &this->mpFailBitmap)))
						{
							this->mFailBitMapLoaded = true;
						}
					}
				}
			}
		}
	}
	return hr;
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
