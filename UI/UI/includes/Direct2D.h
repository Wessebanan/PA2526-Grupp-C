#pragma once

#include <map>
#include<unordered_map>
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

#define BITMAP_NAME_LENGTH 10
#define COLOR_BRUSHES 5
//struct BitmapInfo
//{
//	ID2D1Bitmap* bitmap = nullptr;
//	//std::string name = "";
//	D2D1_RECT_F drawArea = { 0,0,0,0 };
//};
enum brushColors
{
	Black = 0,
	White = 1,
	Red = 2,
	Blue = 3,
	Green = 4
};
static IDGenerator<ID> idGen(0);
struct cmp_str
{
	bool operator()(char const* s1, char const* s2) const
	{
		int sz = BITMAP_NAME_LENGTH;
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
};


class Direct2D
{
public:
	Direct2D();
	~Direct2D();

	void CreateHwndRenderTarget(HWND window, RECT* rect);
	void InitDeviceAndContext(IDXGIDevice* dxgiDevice);
	ID2D1DeviceContext* GetpContext();
	void LoadImageToBitmap(std::string imageFilePath, char bitmapName[BITMAP_NAME_LENGTH]/*, D2D1_RECT_F drawRect*/);
	//ID2D1Bitmap* GetBitmapByName(std::string bitmapName);
	ID GetBitmapIDFromName(char* bitmapName);
	ID2D1Bitmap* GetBitmap(ID bitmapID);
	ID GetBrushIDFromName(char* bitmapName);
	//void DrawBitmap();
	void DrawBitmap(ID2D1Bitmap* bitmap, D2D1_RECT_F rect);

	void PrintText(std::string text, RECT rect);
	void PrintText(std::string text, D2D1_RECT_F rect, brushColors color);
	void PrintText(std::string text, int left, int top, int right, int bottom);
	void setTextColor(float r, float g, float b, float a);
	void setDrawColor(float r, float g, float b, float a);
	void setBrushColor(char* name, float r, float g, float b, float a);
	void setTextSize(unsigned int size);
	void setFont(std::string font);
	void drawRect(RECT rect, int thinkness);
	void solidRect(RECT rect);
	int charArrayCompare(char* s1, char* s2, size_t sz);
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
	bool mHwndRenderTargetCreated = false;
	//std::vector<BitmapInfo> mBitmapVector;
	int test = 0;

	using BitmapMap = std::unordered_map<ID, ID2D1Bitmap*>;
	using BitmapPair = std::pair<ID, ID2D1Bitmap*>;

	using BitmapNameToID = std::unordered_map<char*, ID>;
	using BitmapNameIDPair = std::pair<char*, ID>;

	using BrushMap = std::unordered_map<ID, ID2D1SolidColorBrush*>;
	using BrushMapPair = std::pair<ID, ID2D1SolidColorBrush*>;
	using BrushMapName = std::unordered_map<char*, ID>;
	using BrushMapNamePair = std::pair<char*, ID>;
	BitmapMap mBitmapList;
	BitmapNameToID mBitmapNameID;
	BrushMap mBrushMap;
	BrushMapName mBrushMapName;

	ID2D1SolidColorBrush* mColorBrushes[COLOR_BRUSHES];

	void mCreateFactory();
	void mCreateWicFactory();
	void mCreateTextFactory();
	void mCreateTextFormat();
	void mCreateColorText();
	void mCreateColorDraw();
	void mCreateColorBrushes();
	std::wstring mStrToWstrConverter(std::string str);

	void mInit();
};
