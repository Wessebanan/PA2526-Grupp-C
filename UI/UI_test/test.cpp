#include "pch.h"
//#include "InitInputHandler.h"
#include"Direct2D.h"
//#include "ecs.h"
//#include "UISystems.h"
#include<Windows.h>
#include"..//..//Game/GameMain/InitUI.h"
#pragma comment (lib, "plainECS")



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

//TEST(ExpectedTrue, GetBitmapIDFromName) 
//{
//	HWND windowHandle = CreateHwndWindow();
//	Direct2D Dtest;
//	ID myID;
//	RECT rectTest = { 0,0,800,600 };
//	char hehe[10] = "pepe";
//	Dtest.CreateHwndRenderTarget(windowHandle, &rectTest);
//	Dtest.LoadImageToBitmap("PepeLaugh.jfif", hehe);
//	myID = Dtest.GetBitmapIDFromName(hehe); //returns the id that the bitmap has from its name, 
//											//its easier for humans to remember names than ID numbers
//
//	EXPECT_NE(myID, 0);
//}

TEST(ExpectedTrue, GetBitmap) 
{
	HWND windowHandle = CreateHwndWindow();
	Direct2D Dtest;
	ID myID;
	HRESULT hr;
	RECT rectTest = { 0,0,800,600 };
	char hehe[10] = "pepe";
	Dtest.CreateHwndRenderTarget(windowHandle, &rectTest);
	hr = Dtest.LoadImageToBitmap("PepeLaugh.jfif", hehe);
	void* jej = Dtest.GetBitmap(hehe); //Using the ID to get the bitmap

	EXPECT_EQ(hr, S_OK);
	EXPECT_NE(jej, nullptr);

}

TEST(ExpectedTrue, DebugWindow)
{
	HWND windowHandle = CreateHwndWindow();
	Direct2D* p_dtest;
	RECT rectTest = { 0,0,800,600 };
	HRESULT hr = p_dtest->CreateHwndRenderTarget(windowHandle, &rectTest);
	
	using namespace ecs;
	EntityComponentSystem myECS;
	TempUISystemPtrs my_UI_systems;
	InitUI(myECS, &p_dtest, my_UI_systems);
	//systems::UIPreRenderSystem* UIpreSys = myECS.createSystem<systems::UIPreRenderSystem>(0); //Create systems that are needed for ecs debug print
	//systems::UIDebugSystem* UIDebugSys = myECS.createSystem<systems::UIDebugSystem>(9);
	//systems::UIPostRenderSystem* UIpostSys = myECS.createSystem<systems::UIPostRenderSystem>(9);
	//UIpreSys->mpD2D = UIpostSys->mpD2D = UIDebugSys->mpD2D = &Dtest; // Give ECS d2d with render target
	ecs::components::UITextComponent UIText;
	UIText.mStrText = "hehehheehtest";  


	components::KeyboardComponent keyboard;
	keyboard.W = true; //force keyboard key to be true to toggle debug print

	Entity* e1 = myECS.createEntity(UIText);
	Entity* e2 = myECS.createEntity(keyboard);
	myECS.update(0);

	
	EXPECT_EQ(my_UI_systems.UIDebugSys->toRender, true); // check if bool required to print becomes true because of true keyboard

	delete p_dtest;
}

TEST(ExpectedFail, DebugWindow)
{
	HWND windowHandle = CreateHwndWindow();
	Direct2D* p_dtest;
	RECT rectTest = { 0,0,800,600 };
	HRESULT hr = p_dtest->CreateHwndRenderTarget(windowHandle, &rectTest);

	using namespace ecs;
	EntityComponentSystem myECS;

	TempUISystemPtrs my_UI_systems;
	InitUI(myECS, &p_dtest, my_UI_systems);

	components::UITextComponent UIText;
	UIText.mStrText = "hehehheehtest";


	components::KeyboardComponent keyboard;

	Entity* e1 = myECS.createEntity(UIText);
	Entity* e2 = myECS.createEntity(keyboard);
	myECS.update(0);

	EXPECT_EQ(my_UI_systems.UIDebugSys->toRender, false); // check if bool required to print becomes true because of true keyboard

}

TEST(Expectedfalse, FailCreateRenderTarget) // The same tests but expecting fails
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

//TEST(Expectedfalse, FailGetBitmapIDFromName)
//{
//	HWND windowHandle = CreateHwndWindow();
//	Direct2D Dtest;
//	ID myID;
//	RECT rectTest = { 0,0,800,600 };
//	char hehe[10] = "pepe";
//	Dtest.CreateHwndRenderTarget(windowHandle, &rectTest);
//	myID = Dtest.GetBitmapIDFromName(hehe);
//
//
//	EXPECT_EQ(myID, 0);
//}

TEST(Expectedfalse, FailGetBitmap)
{
	HWND windowHandle = CreateHwndWindow();
	Direct2D Dtest;
	ID myID;
	HRESULT hr;
	RECT rectTest = { 0,0,800,600 };
	char hehe[10] = "pepe";
	Dtest.CreateHwndRenderTarget(windowHandle, &rectTest);
	hr = Dtest.LoadImageToBitmap("fdjak", hehe);
	void* jej = Dtest.GetBitmap(hehe);

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
