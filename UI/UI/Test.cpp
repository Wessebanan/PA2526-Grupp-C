#pragma once
//#include "InitInputHandler.h"
//#include <winsock2.h>
//#include <Windows.h>
#include "ecs.h"
#include "Direct2D.h"
#pragma warning(disable:4996)
#include "UISystems.h"
#include "UIComponents.h"
#include "UIEvents.h"
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>


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
int compStr(char* s1, char* s2, size_t sz) {
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
//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
//	LPSTR lpCmdLine, int nCmdShow)
int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
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

	//if (hwnd == NULL)
	//{
	//	MessageBox(NULL, "Window Creation Failed!", "Error!",
	//		MB_ICONEXCLAMATION | MB_OK);
	//	return 0;
	//}
	//char testy[10];
	//std::strcpy(testy, "pepe");

	test2d.CreateHwndRenderTarget(hwnd, &rect);


	//test2d.LoadImageToBitmap("PepeLaugh.jfif",testy,D2D1::RectF(0,0,800,600));
	//test2d.LoadImageToBitmap("PepeLaugh.jfif", D2D1::RectF(100, 100, 700, 500));
	//test2d.LoadImageToBitmap("PepeLaugh.jfif", D2D1::RectF(200, 200, 600, 400));
	//test2d.LoadImageToBitmap("PepeLaugh.jfif", D2D1::RectF(250, 250, 550, 350));
	//test2d.LoadImageToBitmap("PepeLaugh.jfif", D2D1::RectF(300, 275, 500, 325));
	//test2d.LoadImageToBitmap("PepeLaugh.jfif", D2D1::RectF(350, 290, 440, 310));

	ShowWindow(hwnd, 1);
	UpdateWindow(hwnd);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//static int i = 0, k = 0, l = 1;
	//static std::string kek;
	//ID idTest = 333;
	//idTest = test2d.GetBitmapIDFromName(testy);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Step 3: The Message Loop
	


	using namespace ecs;
	EntityComponentSystem myECS;
	systems::UIPreRenderSystem *UIpreSys = myECS.createSystem<systems::UIPreRenderSystem>(0);
	systems::UIBitmapSystem *UIBitmapSys = myECS.createSystem<systems::UIBitmapSystem>();
	systems::UITextSystem* UITextSys = myECS.createSystem<systems::UITextSystem>();
	systems::UIDebugSystem* UIDebugSys = myECS.createSystem<systems::UIDebugSystem>(9);
	systems::UIPostRenderSystem *UIpostSys = myECS.createSystem<systems::UIPostRenderSystem>(9);
	
	
	UIpreSys->mpD2D = UITextSys->mpD2D = UIpostSys->mpD2D = UIBitmapSys->mpD2D = UIDebugSys->mpD2D = &test2d;
	components::UITextComponent UIText;
	components::UIDrawColorComponent UIColor;
	components::UIDrawPosComponent UIPos;
	components::UIDrawPosComponent UIPos2;
	components::UIBitmapComponent UIBitmap;
	UIColor.mColor = brushColors::Green;
	UIPos.mDrawArea = D2D1::RectF(600, 0, 800, 200);
	UIPos2.mDrawArea = D2D1::RectF(1, 1, 799, 599);
	UIText.mStrText = "hehehheehtest";
	char hehe[10] = "pepe";
	//test2d.LoadImageToBitmap("PepeLaugh.jfif", hehe);
	//UIBitmap.bitmap = test2d.GetBitmap(test2d.GetBitmapIDFromName(hehe));

	//Entity* e1 = myECS.createEntity(UIBitmap, UIPos2);
	//Entity* e2 = myECS.createEntity(UIColor, UIText, UIPos);
	Entity* e3 = myECS.createEntity(UIText);

	//InputBackend Input;
	//initInputECS(myECS, &Input);

	

	while (GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//i++;
		//kek = std::to_string(i);
		//test2d.getHwndRenderTarget()->BeginDraw();
		//test2d.getHwndRenderTarget()->Clear();
		myECS.update(0);
		//	D2D1::ColorF(D2D1::ColorF(
		//		0.3f,
		//		0.41f,
		//		0.5f,
		//		1.0f));
		//test2d.DrawBitmap();
		//test2d.PrintText(kek, 600, 0, 800, 200);
		//test2d.PrintText("456		ÒwÓ", 600, 200, 800, 400);
		//test2d.drawRect(RECT{ 600,0,800,200, }, 2);
		//test2d.drawRect(RECT{ 600,200,800,400, }, 2);
		//test2d.solidRect(RECT{ i,150,100 + i,250, }); // i
		//test2d.getHwndRenderTarget()->EndDraw();		

	}
	return Msg.wParam;

}