#include "pch.h"

#include "../includes/Window.h"

#include "../includes/RenderManager.h"
#include "../includes/MeshManager.h"
#include "../includes/ComputeManager.h"

TEST(Graphics, CreatingAndClosingWindow)
{
	graphics::Window wnd;
	HRESULT hr = wnd.Initialize(800, 600, "CreatingAndClosingWindow", graphics::WINDOW_STYLE::BORDER);
	EXPECT_EQ(hr, S_OK);
	EXPECT_TRUE(wnd.Close());
}

TEST(Graphics, UpdateWindow)
{
	graphics::Window wnd;
	wnd.Initialize(800, 600, "UpdateWindow", graphics::WINDOW_STYLE::BORDER);

	wnd.Update();

	wnd.Close();
}

TEST(Graphics, RenderManager)
{
	UINT client_width	= 800;
	UINT client_height	= 600;

	graphics::Window wnd;
	wnd.Initialize(client_width, client_height, "RenderManager", graphics::WINDOW_STYLE::BORDER);

	graphics::RenderManager mng;
	HRESULT hr = mng.Initialize(client_width, client_height, wnd);

	EXPECT_EQ(hr, S_OK);

	mng.Destroy();
	wnd.Close();
}

TEST(Graphics, MeshManager)
{
	graphics::MeshManager mng;
	HRESULT hr = mng.Initialize(999, 999);

	EXPECT_EQ(hr, S_OK);

	mng.Destroy();
}

TEST(Graphics, ComputeManager)
{
	graphics::ComputeManager mng;
	HRESULT hr = mng.Initialize();

	EXPECT_EQ(hr, S_OK);

	mng.Destroy();
}

TEST(Graphics, MultipleManagers)
{
	HRESULT hr			= S_OK;
	UINT client_width	= 800;
	UINT client_height	= 600;

	graphics::Window wnd;
	wnd.Initialize(client_width, client_height, "MultipleManagers", graphics::WINDOW_STYLE::BORDER);

	graphics::RenderManager r_mng;
	hr = r_mng.Initialize(client_width, client_height, wnd);

	EXPECT_EQ(hr, S_OK);


	graphics::ComputeManager c_mng;
	hr = c_mng.Initialize();

	EXPECT_EQ(hr, S_OK);


	graphics::MeshManager m_mng;
	hr = m_mng.Initialize(999, 999);

	EXPECT_EQ(hr, S_OK);


	r_mng.Destroy();
	c_mng.Destroy();
	m_mng.Destroy();

	wnd.Close();
}

TEST(Graphics, CreatePipeline)
{
	UINT
		clientWidth = graphics::window::GetWorkAreaResolution().x,
		clientHeight = graphics::window::GetWorkAreaResolution().y;

	graphics::Window wnd;
	wnd.Initialize(
		clientWidth,
		clientHeight,
		"Couch Commanders",
		graphics::WINDOW_STYLE::BORDERLESS);

	graphics::RenderManager r_mng;
	r_mng.Initialize(clientWidth, clientHeight, wnd);
}

int main(int argc, char** argv)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	::testing::InitGoogleTest(&argc, argv);
	int testsFailed = RUN_ALL_TESTS();

	system("pause");

	return 0;
}