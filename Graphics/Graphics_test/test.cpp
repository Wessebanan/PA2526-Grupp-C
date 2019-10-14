#include "pch.h"

#include "../includes/Window.h"
#include "../includes/RenderManager.h"

TEST(Graphics, CreatingAndClosingWindow)
{
	graphics::Window wnd;
	HRESULT hr = wnd.Initialize(800, 600, "Window", graphics::WINDOW_STYLE::BORDER);
	EXPECT_EQ(hr, S_OK);
	EXPECT_TRUE(wnd.Close());
}

TEST(Graphics, UpdateWindow)
{
	graphics::Window wnd;
	wnd.Initialize(800, 600, "Window1", graphics::WINDOW_STYLE::BORDER);

	wnd.Update();

	wnd.Close();
}

TEST(Graphics, RenderManager)
{
	UINT client_width	= 800;
	UINT client_height	= 600;

	graphics::Window wnd;
	wnd.Initialize(client_width, client_height, "Window2", graphics::WINDOW_STYLE::BORDER);

	graphics::RenderManager mng;
	HRESULT hr = mng.Initialize(client_width, client_height, wnd);

	EXPECT_EQ(hr, S_OK);

	mng.Destroy();
}

int main(int argc, char** argv)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	::testing::InitGoogleTest(&argc, argv);
	int testsFailed = RUN_ALL_TESTS();

	system("pause");

	return 0;
}