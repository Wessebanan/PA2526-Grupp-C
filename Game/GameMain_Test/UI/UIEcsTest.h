#pragma once
#include "../..//GameMain/UI/InitUI.h"
#include <Windows.h>
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

TEST(ExpectedTrue, DebugWindow)
{
	HWND windowHandle = CreateHwndWindow();
	Direct2D* p_dtest;
	RECT rectTest = { 0,0,800,600 };

	using namespace ecs;
	EntityComponentSystem my_ecs;
	TempUISystemPtrs my_UI_systems;
	InitUI(my_ecs, my_UI_systems);//here Direct 2D get created and passed along to UISystems
	//HRESULT hr = p_dtest->CreateHwndRenderTarget(windowHandle, &rectTest);//this have to be after the creation of Direct 2D

	ecs::components::UITextComponent UIText;
	UIText.mStrText = "hehehheehtest";


	components::KeyboardComponent keyboard;
	keyboard.W = true; //force keyboard key to be true to toggle debug print

	Entity* e1 = my_ecs.createEntity(UIText);
	Entity* e2 = my_ecs.createEntity(keyboard);
	my_ecs.update(0);


	EXPECT_EQ(my_UI_systems.UIDebugSys->toRender, true); // check if bool required to print becomes true because of true keyboard

	delete p_dtest;
}

TEST(ExpectedFail, DebugWindow)
{
	HWND windowHandle = CreateHwndWindow();
	Direct2D* p_dtest;
	RECT rectTest = { 0,0,800,600 };

	using namespace ecs;
	EntityComponentSystem my_ecs;

	TempUISystemPtrs my_UI_systems;
	InitUI(my_ecs, my_UI_systems);

	components::UITextComponent UIText;
	UIText.mStrText = "hehehheehtest";


	components::KeyboardComponent keyboard;

	Entity* e1 = my_ecs.createEntity(UIText);
	Entity* e2 = my_ecs.createEntity(keyboard);
	my_ecs.update(0);

	EXPECT_EQ(my_UI_systems.UIDebugSys->toRender, false); // check if bool required to print becomes true because of true keyboard

	delete p_dtest;
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