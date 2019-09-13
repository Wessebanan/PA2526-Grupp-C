#pragma once

#include<ecs.h>
#include<d2d1_3.h>
#include <wincodec.h>
#include <dwrite_3.h>
#include <string>
#include <locale>
#include <vector>
#pragma comment(lib, "Dwrite")
#pragma comment(lib, "windowscodecs")
#pragma comment(lib, "d2d1")
#pragma comment (lib, "d3d11")

struct BitmapInfo
{
	ID2D1Bitmap* bitmap = nullptr;
	std::string name = "";
	D2D1_RECT_F drawArea = { 0,0,0,0 };
};

class Direct2D
{
public:
	Direct2D();
	~Direct2D();

	void CreateHwndRenderTarget(HWND window, RECT* rect);
	void InitDeviceAndContext(IDXGIDevice* dxgiDevice);
	ID2D1DeviceContext* GetpContext();
	void LoadImageToBitmap(std::string imageFilePath, D2D1_RECT_F drawRect);
	ID2D1Bitmap* GetBitmapByName(std::string bitmapName);
	void DrawBitmap();

	void PrintText(std::string test, RECT rect);
	void PrintText(std::string test, int left, int top, int right, int bottom);
	void setTextColor(float r, float g, float b, float a);
	void setDrawColor(float r, float g, float b, float a);
	void setTextSize(unsigned int size);
	void setFont(std::string font);
	void drawRect(RECT rect, int thinkness);
	void solidRect(RECT rect);
	ID2D1HwndRenderTarget* getHwndRenderTarget();


private:
	IWICFormatConverter* mpFormatConverter;
	IWICImagingFactory* mpWicFactory;
	IWICBitmapDecoder* mpDecoder;
	IWICBitmapFrameDecode* mpBitmapSrc;
	ID2D1HwndRenderTarget* mpHwndRenderTarget;
	ID2D1SolidColorBrush* mColorText;
	ID2D1Bitmap* bitmap;
	ID2D1SolidColorBrush* mColorDraw;
	
	IDWriteFactory7* mpTextFactory;
	DWRITE_TRIMMING mTrimmer;
	IDWriteTextFormat* mpTextFormat;

	ID2D1Factory7* mpFactory;
	ID2D1Device6* mpDevice;
	ID2D1DeviceContext6* mpContext;
	IWICBitmapDecoderInfo* info;

	RECT* mpRect;
	std::wstring mfont;
	unsigned int mfontSize;
	bool mBitMapLoaded = false;
	std::vector<BitmapInfo> mBitmapVector;
	int test = 0;

	void mCreateFactory();
	void mCreateWicFactory();
	void mCreateTextFactory();
	void mCreateTextFormat();
	void mCreateColorText();
	void mCreateColorDraw();
	std::wstring mStrToWstrConverter(std::string str);

	void mInit();
};
