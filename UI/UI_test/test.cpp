#include "pch.h"
#include"Direct2D.h"
#include<Windows.h>




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
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

TEST(ExpectedTrue, CreateRenderTarget) 
{
	HWND window_handle = CreateHwndWindow(); //create HWND needed for rendertarget
	Direct2D d_test;
	HRESULT hr;
	RECT rect_test = { 0,0,800,600 }; //used for the size of the window so rendertarget is the whole window
	hr = d_test.CreateHwndRenderTarget(window_handle, &rect_test);


	EXPECT_EQ(hr, S_OK);
}

TEST(ExpectedTrue, LoadImageToBitmap) 
{
	HWND window_handle = CreateHwndWindow();
	Direct2D d_test;
	HRESULT hr;
	RECT rect_test = { 0,0,800,600 };
	char hehe[10] = "pepe";
	d_test.CreateHwndRenderTarget(window_handle, &rect_test);
	hr = d_test.LoadImageToBitmap("PepeLaugh.jfif", hehe);


	EXPECT_EQ(hr, S_OK);
}

TEST(ExpectedTrue, GetBitmap) 
{
	HWND window_handle = CreateHwndWindow();
	Direct2D d_test;
	//ID myID;
	HRESULT hr;
	RECT rect_test = { 0,0,800,600 };
	char hehe[10] = "pepe";
	d_test.CreateHwndRenderTarget(window_handle, &rect_test);
	hr = d_test.LoadImageToBitmap("PepeLaugh.jfif", hehe);
	void* jej = d_test.GetBitmap(hehe); //Using the name to get the bitmap

	EXPECT_EQ(hr, S_OK);
	EXPECT_NE(jej, nullptr);

}

TEST(Expectedfalse, FailCreateRenderTarget) // The same tests but expecting fails
{
	HWND window_handle = nullptr;
	Direct2D d_test;
	HRESULT hr;
	RECT rect_test = { 0,0,800,600 };
	hr = d_test.CreateHwndRenderTarget(window_handle, &rect_test);

	EXPECT_NE(hr, S_OK);
} 

TEST(Expectedfalse, FailLoadImageToBitmap)
{
	HWND window_handle = CreateHwndWindow();
	Direct2D d_test;
	HRESULT hr;
	RECT rectTest = { 0,0,800,600 };
	char hehe[10] = "pepe";
	d_test.CreateHwndRenderTarget(window_handle, &rectTest);
	hr = d_test.LoadImageToBitmap("PepeLaugh", hehe);


	EXPECT_NE(hr, S_OK);
}

TEST(Expectedfalse, FailGetBitmap)
{
	HWND window_handle = CreateHwndWindow();
	Direct2D d_test;
	//ID myID;
	HRESULT hr;
	RECT rectTest = { 0,0,800,600 };
	char hehe[10] = "pepe";
	d_test.CreateHwndRenderTarget(window_handle, &rectTest);
	hr = d_test.LoadImageToBitmap("fdjak", hehe);
	void* jej = d_test.GetBitmap(hehe);

	EXPECT_NE(hr, S_OK);
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
