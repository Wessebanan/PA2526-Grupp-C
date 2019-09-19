#include "pch.h"
#include"Direct2D.h"
#include<winsock2.h>
#include<Windows.h>
#pragma comment (lib, "plainECS")
#pragma comment (lib, "UI")


const char g_szClassName[] = "myWindowClass";
HWND CreateHwndWindow();
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}
int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

TEST(ExpectedTrue, CreateRenderTarget) 
{
	HWND windowHandle = CreateHwndWindow(); //create HWND needed for rendertarget
	Direct2D Dtest;
	HRESULT hr;
	RECT rectTest = { 0,0,800,600 }; //used for the size of the window so rendertarget is the whole window
	hr = Dtest.CreateHwndRenderTarget(windowHandle, &rectTest);


	EXPECT_EQ(hr, S_OK);
}

TEST(ExpectedTrue, LoadImageToBitmap) 
{
	HWND windowHandle = CreateHwndWindow();
	Direct2D Dtest;
	HRESULT hr;
	RECT rectTest = { 0,0,800,600 };
	char hehe[10] = "pepe";
	Dtest.CreateHwndRenderTarget(windowHandle, &rectTest);
	hr = Dtest.LoadImageToBitmap("PepeLaugh.jfif", hehe);


	EXPECT_EQ(hr, S_OK);
}

TEST(ExpectedTrue, GetBitmapIDFromName) 
{
	HWND windowHandle = CreateHwndWindow();
	Direct2D Dtest;
	ID myID;
	RECT rectTest = { 0,0,800,600 };
	char hehe[10] = "pepe";
	Dtest.CreateHwndRenderTarget(windowHandle, &rectTest);
	Dtest.LoadImageToBitmap("PepeLaugh.jfif", hehe);
	myID = Dtest.GetBitmapIDFromName(hehe); //returns the id that the bitmap has from its name, 
											//its easier for humans to remember names than ID numbers

	EXPECT_NE(myID, 0);
}

TEST(ExpectedTrue, GetBitmap) 
{
	HWND windowHandle = CreateHwndWindow();
	Direct2D Dtest;
	ID myID;
	RECT rectTest = { 0,0,800,600 };
	char hehe[10] = "pepe";
	Dtest.CreateHwndRenderTarget(windowHandle, &rectTest);
	Dtest.LoadImageToBitmap("PepeLaugh.jfif", hehe);
	myID = Dtest.GetBitmapIDFromName(hehe);
	void* jej = Dtest.GetBitmap(myID); //Using the ID to get the bitmap


	EXPECT_NE(jej, nullptr);

}

TEST(Expectedfalse, FailCreateRenderTarget) // The same tests bit expecting fails
{
	HWND windowHandle = nullptr;
	Direct2D Dtest;
	HRESULT hr;
	RECT rectTest = { 0,0,800,600 };
	hr = Dtest.CreateHwndRenderTarget(windowHandle, &rectTest);

	EXPECT_NE(hr, S_OK);
} 

TEST(Expectedfalse, FailLoadImageToBitmap)
{
	HWND windowHandle = CreateHwndWindow();
	Direct2D Dtest;
	HRESULT hr;
	RECT rectTest = { 0,0,800,600 };
	char hehe[10] = "pepe";
	Dtest.CreateHwndRenderTarget(windowHandle, &rectTest);
	hr = Dtest.LoadImageToBitmap("PepeLaugh", hehe);


	EXPECT_NE(hr, S_OK);
}

TEST(Expectedfalse, FailGetBitmapIDFromName)
{
	HWND windowHandle = CreateHwndWindow();
	Direct2D Dtest;
	ID myID;
	RECT rectTest = { 0,0,800,600 };
	char hehe[10] = "pepe";
	Dtest.CreateHwndRenderTarget(windowHandle, &rectTest);
	myID = Dtest.GetBitmapIDFromName(hehe);


	EXPECT_EQ(myID, 0);
}

TEST(Expectedfalse, FailGetBitmap) //pass if d2d1 function GetBitMap returns nullptr (not if returns failmap that is default now)
{
	HWND windowHandle = CreateHwndWindow();
	Direct2D Dtest;
	ID myID;
	RECT rectTest = { 0,0,800,600 };
	char hehe[10] = "pepe";
	Dtest.CreateHwndRenderTarget(windowHandle, &rectTest);
	myID = Dtest.GetBitmapIDFromName(hehe);
	void* jej = Dtest.GetBitmap(myID);


	EXPECT_EQ(jej, nullptr);
}



HWND CreateHwndWindow()
{
	HINSTANCE hInstance = GetModuleHandle(NULL);
	WNDCLASSEX wc;
	HWND hwnd;
	MSG Msg;

	//Step 1: Registering the Window Class
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = L"myWindowClass";
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	RegisterClassEx(&wc);
	RECT rect = { 0, 0, 800, 600 };

	//AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, false, WS_EX_OVERLAPPEDWINDOW);
	// Step 2: Creating the Window
	hwnd = CreateWindowEx(
		WS_EX_OVERLAPPEDWINDOW,
		L"myWindowClass",
		L"myWindowClass",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top,
		NULL, NULL, hInstance, NULL);
	return hwnd;
}
