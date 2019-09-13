#include<ecs.h>
#include<Direct2D.h>
#include <Windows.h>

const char g_szClassName[] = "myWindowClass";
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
//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
//	LPSTR lpCmdLine, int nCmdShow)
int main()
{
	using namespace ecs;
	EntityComponentSystem myECS;

	// ----------------------------------

	HINSTANCE hInstance = GetModuleHandle(NULL);
	Direct2D test2d;
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
	wc.lpszClassName = g_szClassName;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Window Registration Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	RECT rect = { 0, 0, 800, 600 };

	//AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, false, WS_EX_OVERLAPPEDWINDOW);
	// Step 2: Creating the Window
	hwnd = CreateWindowEx(
		WS_EX_OVERLAPPEDWINDOW,
		g_szClassName,
		"The title of my window",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top,
		NULL, NULL, hInstance, NULL);

	if (hwnd == NULL)
	{
		MessageBox(NULL, "Window Creation Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}
	test2d.CreateHwndRenderTarget(hwnd, &rect);
	test2d.LoadImageToBitmap("PepeLaugh.jfif",D2D1::RectF(0,0,800,600));
	test2d.LoadImageToBitmap("PepeLaugh.jfif", D2D1::RectF(100, 100, 700, 500));
	test2d.LoadImageToBitmap("PepeLaugh.jfif", D2D1::RectF(200, 200, 600, 400));
	test2d.LoadImageToBitmap("PepeLaugh.jfif", D2D1::RectF(250, 250, 550, 350));
	test2d.LoadImageToBitmap("PepeLaugh.jfif", D2D1::RectF(300, 275, 500, 325));
	test2d.LoadImageToBitmap("PepeLaugh.jfif", D2D1::RectF(350, 290, 440, 310));
	ShowWindow(hwnd, 1);
	UpdateWindow(hwnd);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	static int i = 0, k = 0, l = 1;
	static std::string kek;
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Step 3: The Message Loop
	while (GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//i++;
		//kek = std::to_string(i);
		test2d.getHwndRenderTarget()->BeginDraw();
		test2d.getHwndRenderTarget()->Clear();
			D2D1::ColorF(D2D1::ColorF(
				0.3f,
				0.41f,
				0.5f,
				1.0f));
		test2d.DrawBitmap();
		test2d.PrintText(kek, 600, 0, 800, 200);
		test2d.PrintText("456		ÒwÓ", 600, 200, 800, 400);
		test2d.drawRect(RECT{ 600,0,800,200, }, 2);
		test2d.drawRect(RECT{ 600,200,800,400, }, 2);
		test2d.solidRect(RECT{ i,150,100 + i,250, }); // i
		test2d.getHwndRenderTarget()->EndDraw();		

	}
	return Msg.wParam;
}