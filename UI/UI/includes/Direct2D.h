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
#define COLOR_BRUSHES 6
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
	Green = 4,
	Purple = 5
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
	
	HRESULT CreateHwndRenderTarget(HWND window, RECT* rect);
	void InitDeviceAndContext(IDXGIDevice* dxgiDevice); //Takes dxgidevice from dx11 and creates d2d device and device context
	ID2D1DeviceContext* GetpContext();
	HRESULT LoadImageToBitmap(std::string imageFilePath, char bitmapName[BITMAP_NAME_LENGTH]);
	ID GetBitmapIDFromName(char* bitmapName); //returns bitmap ID
	ID2D1Bitmap* GetBitmap(ID bitmapID); //returns bitmap
	ID GetBrushIDFromName(char* bitmapName); //not in use right now
	bool DrawBitmap(ID2D1Bitmap* bitmap, D2D1_RECT_F rect);

	//ID2D1Bitmap* GetBitmapByName(std::string bitmapName); //used to draw all bitmaps, uses the BitmapInfo struct
	//void DrawBitmap();

	bool PrintText(std::string text, RECT rect);
	bool PrintDebug(std::string text); // debug printer thing
	bool PrintText(std::string text, D2D1_RECT_F rect, brushColors color); //only one used in ECS atm
	bool PrintText(std::string text, int left, int top, int right, int bottom);

	void setTextColor(float r, float g, float b, float a); //not in use atm
	void setDrawColor(float r, float g, float b, float a); //not in use atm
	void setBrushColor(char* name, float r, float g, float b, float a); //not in use atm
	HRESULT setTextSize(unsigned int size); //Don't use in a loop because it creates a new textformat
	HRESULT setFont(std::string font); //Don't use in a loop because it creates a new textformat
	bool drawRect(D2D1_RECT_F rect, int thinkness, brushColors color); //draws a rect border/wireframe
	bool solidRect(D2D1_RECT_F rect, brushColors color); //draws a filled solid rect
	int charArrayCompare(char* s1, char* s2, size_t sz); //compares two char arrays
	ID2D1HwndRenderTarget* getHwndRenderTarget();


private:
	IWICFormatConverter* mpFormatConverter; //used to change image to bitmap
	IWICImagingFactory* mpWicFactory; //factory for bitmaps
	IWICBitmapDecoder* mpDecoder; //used to change image to bitmap
	IWICBitmapFrameDecode* mpBitmapSrc; //used to change image to bitmap
	ID2D1HwndRenderTarget* mpHwndRenderTarget; //window as rendertarget
	ID2D1SolidColorBrush* mpColorText; //used for color
	ID2D1SolidColorBrush* mpColorDraw;
	ID2D1Bitmap* mpFailBitmap;
	
	IDWriteFactory7* mpTextFactory; //factory used for text
	DWRITE_TRIMMING mTrimmer; //used for text format
	IDWriteTextFormat* mpTextFormat; //things like font and size
	IDWriteTextFormat* mpDebugTextFormat;

	ID2D1Factory7* mpFactory; //d2d1 factory
	ID2D1Device6* mpDevice;
	ID2D1DeviceContext6* mpContext;

	RECT* mpRect;
	std::wstring mfont;
	unsigned int mfontSize;
	bool mFailBitMapLoaded = false;
	bool mHwndRenderTargetCreated = false;
	//std::vector<BitmapInfo> mBitmapVector;
	int test = 0;

	using BitmapMap = std::unordered_map<ID, ID2D1Bitmap*>; //unordered map of bitmaps
	using BitmapPair = std::pair<ID, ID2D1Bitmap*>;
	BitmapMap mBitmapList;

	using BitmapNameToID = std::unordered_map<char*, ID>; //unordered map of bitmap names
	using BitmapNameIDPair = std::pair<char*, ID>;
	BitmapNameToID mBitmapNameID;

	using BrushMap = std::unordered_map<ID, ID2D1SolidColorBrush*>; //unordered map of brushes (not in use now)
	using BrushMapPair = std::pair<ID, ID2D1SolidColorBrush*>;
	BrushMap mBrushMap;

	using BrushMapName = std::unordered_map<char*, ID>; //corresponding brush name map
	using BrushMapNamePair = std::pair<char*, ID>;
	BrushMapName mBrushMapName;


	ID2D1SolidColorBrush* mColorBrushes[COLOR_BRUSHES]; //array of brushes we use now

	void mCreateFactory();
	void mCreateWicFactory();
	void mCreateTextFactory();
	HRESULT mCreateTextFormat(std::wstring font, int size, IDWriteTextFormat** format);
	HRESULT mCreateColorText();
	HRESULT mCreateColorDraw();
	HRESULT mCreateColorBrushes();
	HRESULT LoadImageToBitmap(std::string imageFilePath);
	std::wstring mStrToWstrConverter(std::string str); //covert string to wstring

	void mInit(); //used to init factories
};
