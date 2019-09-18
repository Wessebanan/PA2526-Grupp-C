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
	this->mFailBitmap = nullptr;
	this->mpTextFormat = nullptr;
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
		hr = this->mCreateTextFormat();
		hr = this->mCreateColorBrushes();
		this->LoadImageToBitmap("fail.png");
		this->mpTextFormat->AddRef();
		this->mColorText->AddRef();
		this->mColorDraw->AddRef();
		this->mpRect = rect;
	};
	
	return hr;
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

HRESULT Direct2D::LoadImageToBitmap(std::string imageFilePath, char bitmapName[BITMAP_NAME_LENGTH]/*, D2D1_RECT_F drawRect*/) //loads an image to a bitmap map with infostruct and ID
{
	HRESULT hr = E_FAIL;
	//BitmapInfo new_bitmap_struct;
	ID2D1Bitmap* new_bitmap = nullptr;
	//new_bitmap_struct.name = imageFilePath;
	//new_bitmap_struct.drawArea = drawRect;
	std::wstring w_str = this->mStrToWstrConverter(imageFilePath);
	if (this->mHwndRenderTargetCreated)
	{
		if (SUCCEEDED(hr = this->mpWicFactory->CreateFormatConverter(&this->mpFormatConverter)))
		{
			if (SUCCEEDED(hr = this->mpWicFactory->CreateDecoderFromFilename(w_str.c_str(), NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &this->mpDecoder)))
			{
				if (SUCCEEDED(hr = this->mpDecoder->GetFrame(0, &this->mpBitmapSrc)))
				{
					if (SUCCEEDED(hr = this->mpFormatConverter->Initialize(this->mpBitmapSrc, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 1.f, WICBitmapPaletteTypeMedianCut)))
					{
						if (SUCCEEDED(hr = this->mpHwndRenderTarget->CreateBitmapFromWicBitmap(this->mpFormatConverter, &new_bitmap)))
						{
							ID newID = idGen.generateID();

							this->mBitmapList[newID] = new_bitmap;
							this->mBitmapNameID[bitmapName] = newID;
							
						}
					}
				}
			}
		}
	}
	return hr;
	//this->mBitmapVector.push_back(new_bitmap_struct);
}

//ID2D1Bitmap* Direct2D::GetBitmapByName(std::string bitmapName)//------------------------------------------------------------------------------------------------------------------
//{
//	ID2D1Bitmap* to_return = nullptr; // Default return is nullptr
//	std::vector<BitmapInfo>::iterator it;
//
//
//	IDGenerator<ID> idGen(0);
//	ID newID = idGen.generateID();
//	mBitMapList_ksjdhfsd[newID] = BitmapInfo();
//
//	//std::map<char[10], ID> nameToIDAccess;
//
//	// Check if NOT exist already
//	if (!mBitMapList_ksjdhfsd.count(newID))
//	{
//		// return 
//	}
//
//	for (BitMapPair pair : mBitMapList_ksjdhfsd)
//	{
//		pair.first; // ID
//		pair.second; // BitmapInfo
//	}
//
//
//
//	for (it = this->mBitmapVector.begin();
//		it != this->mBitmapVector.end();
//		++it)
//	{
//		if (bitmapName.compare((*it).name) == 0) // Bitmap with correct name found
//		{
//			to_return = (*it).bitmap; // Return pointer to button
//			it = this->mBitmapVector.end() - 1; // Set iterator to end
//												// -1 because incrementation is performed after this.
//												// Incrementing on .end() is a baaad idea.
//		}
//	}
//	return to_return;
//}

ID Direct2D::GetBitmapIDFromName(char* bitmapName)
{
	ID toReturn = 0;
	for (BitmapNameIDPair pair : this->mBitmapNameID)
	{
		if(this->charArrayCompare(pair.first, bitmapName, BITMAP_NAME_LENGTH))
			return pair.second;
	}
	return toReturn;
}

ID2D1Bitmap* Direct2D::GetBitmap(ID bitmapID)
{
	if (bitmapID != 0)
	{
		return this->mBitmapList.at(bitmapID);
	}
	else
	{
		return this->mFailBitmap;
	}
}

ID Direct2D::GetBrushIDFromName(char* bitmapName)
{
	ID toReturn = 0;
	for (BrushMapNamePair pair : this->mBrushMapName)
	{
		if (this->charArrayCompare(pair.first, bitmapName, BITMAP_NAME_LENGTH))
			return pair.second;
	}
	return toReturn;
}

bool Direct2D::DrawBitmap(ID2D1Bitmap* bitmap, D2D1_RECT_F rect)
{
	bool canDraw = false;
	if (this->mHwndRenderTargetCreated && bitmap != nullptr)
	{
		canDraw = true;
		this->mpHwndRenderTarget->DrawBitmap(bitmap, rect, 1, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, D2D1::RectF(0, 0, bitmap->GetSize().width, bitmap->GetSize().height));
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
	if (this->mHwndRenderTargetCreated)
	{
		this->mpHwndRenderTarget->DrawTextA(w_str.c_str(), wcslen(w_str.c_str()), this->mpTextFormat, layoutRect, this->mColorText);
	}
	return this->mHwndRenderTargetCreated;
}

bool Direct2D::PrintText(std::string text, D2D1_RECT_F rect, brushColors color)
{
	std::wstring w_str = this->mStrToWstrConverter(text); //Converts string to widestring
	if (this->mHwndRenderTargetCreated)
	{
		this->mpHwndRenderTarget->DrawTextA(w_str.c_str(), wcslen(w_str.c_str()), this->mpTextFormat, rect, this->mColorBrushes[color]);
	}
	return this->mHwndRenderTargetCreated;
}

bool Direct2D::PrintText(std::string text, int left, int top, int right, int bottom)//draws text with format, draw area and color
{
	std::wstring w_str = this->mStrToWstrConverter(text);

	D2D1_RECT_F layoutRect = D2D1::RectF(left, top, right, bottom);
	if (this->mHwndRenderTargetCreated)
	{
		this->mpHwndRenderTarget->DrawTextA(w_str.c_str(), wcslen(w_str.c_str()), this->mpTextFormat, layoutRect, this->mColorText);
	}
	return this->mHwndRenderTargetCreated;
}

void Direct2D::setTextColor(float r, float g, float b, float a)
{
	this->mColorText->SetColor(D2D1::ColorF(r, g, b, a));
}

void Direct2D::setDrawColor(float r, float g, float b, float a)
{
	this->mColorDraw->SetColor(D2D1::ColorF(r, g, b, a));
}

void Direct2D::setBrushColor(char* name, float r, float g, float b, float a)
{
	ID searchID = this->GetBrushIDFromName(name);
	this->mBrushMap.at(searchID)->SetColor(D2D1::ColorF(r, g, b, a));
}

HRESULT Direct2D::setTextSize(unsigned int size)//updates the text format
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
	return hr;
}

HRESULT Direct2D::setFont(std::string font)//updates the text format
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
	return hr;
}

bool Direct2D::drawRect(D2D1_RECT_F rect, int thickness, brushColors color) //draws a rect with line thickness and color
{
	if (this->mHwndRenderTargetCreated)
	{
		this->mpHwndRenderTarget->DrawRectangle(
			D2D1::RectF(rect.left, rect.top, rect.right, rect.bottom),
			this->mColorBrushes[color],
			thickness);
	}
	return this->mHwndRenderTargetCreated;
}

bool Direct2D::solidRect(D2D1_RECT_F rect, brushColors color) //draws a filled rect with color
{
	if (this->mHwndRenderTargetCreated)
	{
		this->mpHwndRenderTarget->FillRectangle(
			D2D1::RectF(rect.left, rect.top, rect.right, rect.bottom),
			this->mColorBrushes[color]);
	}
	return this->mHwndRenderTargetCreated;
}

int Direct2D::charArrayCompare(char* s1, char* s2, size_t sz) //A compares char arrays
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
	HRESULT hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory7), reinterpret_cast<IUnknown**>(&this->mpTextFactory));
}

HRESULT Direct2D::mCreateTextFormat()
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
	return hr;
}

HRESULT Direct2D::mCreateColorText()
{
	HRESULT hr = E_FAIL;
	if (this->mHwndRenderTargetCreated)
	{ 
		hr = this->mpHwndRenderTarget->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::DarkGoldenrod),
		&this->mColorText);
	}
	ID newID = idGen.generateID();
	char name[10] = "text";
	this->mBrushMap[newID] = this->mColorText;
	this->mBrushMapName[name] = newID;
	return hr;
}

HRESULT Direct2D::mCreateColorDraw()
{
	HRESULT hr = E_FAIL;
	if (this->mHwndRenderTargetCreated)
	{
		hr = this->mpHwndRenderTarget->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::DarkGoldenrod),
		&this->mColorDraw);
	}
	ID newID = idGen.generateID();
	char name[10] = "draw";
	this->mBrushMap[newID] = this->mColorDraw;
	this->mBrushMapName[name] = newID;
	return hr;
}

HRESULT Direct2D::mCreateColorBrushes()
{
	HRESULT hr = E_FAIL;
	if (this->mHwndRenderTargetCreated)
	{
		hr = this->mpHwndRenderTarget->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::Black),
			&this->mColorBrushes[0]);
		hr = this->mpHwndRenderTarget->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::White), //Color does not have same enum as defined
			&this->mColorBrushes[1]);
		hr = this->mpHwndRenderTarget->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::Red),
			&this->mColorBrushes[2]);
		hr = this->mpHwndRenderTarget->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::Blue),
			&this->mColorBrushes[3]);
		hr = this->mpHwndRenderTarget->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::Green),
			&this->mColorBrushes[4]);
	}
	return hr;
}

HRESULT Direct2D::LoadImageToBitmap(std::string imageFilePath)
{
	HRESULT hr = E_FAIL;
	
	std::wstring w_str = this->mStrToWstrConverter(imageFilePath);
	if (this->mHwndRenderTargetCreated)
	{
		if (SUCCEEDED(hr = this->mpWicFactory->CreateFormatConverter(&this->mpFormatConverter)))
		{
			if (SUCCEEDED(hr = this->mpWicFactory->CreateDecoderFromFilename(w_str.c_str(), NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &this->mpDecoder)))
			{
				if (SUCCEEDED(hr = this->mpDecoder->GetFrame(0, &this->mpBitmapSrc)))
				{
					if (SUCCEEDED(hr = this->mpFormatConverter->Initialize(this->mpBitmapSrc, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 1.f, WICBitmapPaletteTypeMedianCut)))
					{
						if (SUCCEEDED(hr = this->mpHwndRenderTarget->CreateBitmapFromWicBitmap(this->mpFormatConverter, &this->mFailBitmap)))
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
