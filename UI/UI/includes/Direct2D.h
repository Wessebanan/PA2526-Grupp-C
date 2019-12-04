#pragma once

#include <unordered_map>
#include <d2d1_3.h>
#include <wincodec.h>
#include <dwrite_3.h>
#include <string>
#include <locale>
#include <vector>

#pragma comment(lib, "Dwrite")
#pragma comment(lib, "windowscodecs")
#pragma comment(lib, "d2d1")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "dxguid")

#define BITMAP_NAME_LENGTH 16
#define COLOR_BRUSHES 8
#define NR_OF_FORMATS 3
//struct BitmapInfo
//{
//	ID2D1Bitmap* bitmap = nullptr;
//	//std::string name = "";
//	D2D1_RECT_F drawArea = { 0,0,0,0 };
//};
enum brushColors
{
	Black	= 0,
	White	= 1,
	Red		= 2,
	Blue	= 3,
	Green	= 4,
	Purple	= 5,
	Gray	= 6,
	Cyan	= 7
};
enum text_sizes
{
	small_text			= 0,
	medium_text			= 1,
	large_text			= 2,
	large_text_center	= 3
};

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
	
	HRESULT CreateHwndRenderTarget(HWND window, RECT* rect);
	HRESULT CreateHwndRenderTarget(HWND window, int width, int height);

	void InitDeviceAndContext(IDXGIDevice* dxgiDevice); //Takes dxgidevice from dx11 and creates d2d device and device context
	ID2D1DeviceContext* GetpContext();
	//Loads an image from a filepath into a bitmap and returns that bitmap, if fail return nullptr
	ID2D1Bitmap1* LoadImageToBitmap(std::string imageFilePath, char bitmapName[BITMAP_NAME_LENGTH]);
	ID2D1Bitmap1* LoadImageToBitmap(std::string imageFilePath, std::string bitmapName);
	ID2D1Bitmap1* CreateBitmapTarget(float width, float height);
	//Returns a bitmap that has the same char name
	ID2D1Bitmap1* GetBitmap(char* bitmapName);//returns bitmap
	//Returns a bitmap that has the same string name
	ID2D1Bitmap1* GetBitmap(std::string bitmapName);//returns bitmap
	//Returns the bitmap taken from the backbuffer which contains the resolution
	ID2D1Bitmap1* GetBackbufferBitmap();
	void SetBitmapTint(ID2D1Bitmap1* bitmapInput, ID2D1Bitmap1* bitmapOutput, int x, int y, int z, int w = 255);
	void SetBackbufferBitmap(ID2D1Bitmap1* backbuffer_bitmap);
	ID2D1SolidColorBrush* GetBrushFromName(char* brushName);
	//ID GetBrushIDFromName(char* bitmapName); //not in use right now by ECS
	bool DrawBitmap(ID2D1Bitmap* bitmap, D2D1_RECT_F rect);


	bool PrintText(std::wstring text, RECT rect);
	bool PrintDebug(std::wstring text); // debug printer thing
	bool PrintText(std::wstring text, D2D1_RECT_F rect, brushColors color, int size = 2); //only one used in ECS atm
	bool PrintText(std::wstring text, int left, int top, int right, int bottom);

	void SetTextColor(float r, float g, float b, float a); //not in use atm by ECS
	void SetDrawColor(float r, float g, float b, float a); //not in use atm by ECS
	void SetBrushColor(char* name, float r, float g, float b, float a); //not in use atm by ECS
	//HRESULT setTextSize(unsigned int size); //Don't use in a loop because it creates a new textformat
	//HRESULT setFont(std::string font); //Don't use in a loop because it creates a new textformat
	bool DrawRect(D2D1_RECT_F rect, int thinkness, brushColors color); //draws a rect border/wireframe
	bool SolidRect(D2D1_RECT_F rect, brushColors color); //draws a filled solid rect
	int CharArrayCompare(char* s1, char* s2, size_t sz); //compares two char arrays
	ID2D1HwndRenderTarget* GetHwndRenderTarget();


private:
	IWICFormatConverter* mpFormatConverter; //used to change image to bitmap
	IWICImagingFactory* mpWicFactory; //factory for bitmaps
	IWICBitmapDecoder* mpDecoder; //used to change image to bitmap
	IWICBitmapFrameDecode* mpBitmapSrc; //used to change image to bitmap
	ID2D1HwndRenderTarget* mpHwndRenderTarget; //window as rendertarget
	ID2D1SolidColorBrush* mpColorText; //used for color
	ID2D1SolidColorBrush* mpColorDraw;
	ID2D1Bitmap* mpFailBitmap;
	ID2D1Bitmap1* mpBackbufferBitmap;
	ID2D1Effect* mpTintEffect;
	
	IDWriteFactory7* mpTextFactory; //factory used for text
	DWRITE_TRIMMING mTrimmer; //used for text format
	//IDWriteTextFormat* mpTextFormat; //things like font and size
	IDWriteTextFormat* mpDebugTextFormat;
	IDWriteTextFormat* mpTextFormats[NR_OF_FORMATS];

	ID2D1Factory1* mpFactory; //d2d1 factory
	ID2D1Device* mpDevice;
	ID2D1DeviceContext* mpContext;

	RECT* mpRect;
	int mWidth;
	int mHeight;
	std::wstring mfont;
	unsigned int mfontSize;
	bool mFailBitMapLoaded = false;
	bool mHwndRenderTargetCreated = false;
	bool mDeviceContextCreated = false;
	//std::vector<BitmapInfo> mBitmapVector;

	using BitmapMapChar = std::unordered_map<char*, ID2D1Bitmap1*>; //unordered map of bitmaps
	using BitmapPairChar = std::pair<char*, ID2D1Bitmap1*>;
	BitmapMapChar mBitmapListChar;

	using BitmapMap = std::unordered_map<std::string, ID2D1Bitmap1*>; //unordered map of bitmaps
	using BitmapPair = std::pair<std::string, ID2D1Bitmap1*>;
	BitmapMap mBitmapList;

	using BrushMap = std::unordered_map<char*, ID2D1SolidColorBrush*>; //unordered map of brushes (not in use now)
	using BrushMapPair = std::pair<char*, ID2D1SolidColorBrush*>;
	BrushMap mBrushMap;

	ID2D1SolidColorBrush* mColorBrushes[COLOR_BRUSHES]; //array of brushes we use now

	void mCreateFactory();
	void mCreateWicFactory();
	void mCreateTextFactory();
	HRESULT mCreateTextFormat(std::wstring font, int size, IDWriteTextFormat** format);
	HRESULT mCreateColorText();
	HRESULT mCreateColorDraw();
	HRESULT mCreateColorBrushes();
	HRESULT mCreateTextFormats();
	HRESULT LoadImageToBitmap(std::string imageFilePath);
	std::wstring mStrToWstrConverter(std::string str); //covert string to wstring

	void mInit(); //used to init factories
};
