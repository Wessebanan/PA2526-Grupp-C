#include "pch.h"

#include "../../pch.h"

#include "../includes/Window.h"

#include "../includes/RenderManager.h"
#include "../includes/MeshManager.h"
#include "../includes/ComputeManager.h"

#include "../includes/ForwardRenderingPipeline.h"

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
	HRESULT hr = mng.Initialize(wnd);

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
	hr = r_mng.Initialize(wnd);

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
		"CreatePipeline",
		graphics::WINDOW_STYLE::BORDERLESS);

	graphics::RenderManager r_mng;
	r_mng.Initialize(wnd);
}

TEST(Graphics, CreateRenderProgramCorrectFilepath)
{
	UINT
		clientWidth = graphics::window::GetWorkAreaResolution().x,
		clientHeight = graphics::window::GetWorkAreaResolution().y;

	graphics::Window wnd;
	wnd.Initialize(
		clientWidth,
		clientHeight,
		"CreateRenderProgramCorrectFilepath",
		graphics::WINDOW_STYLE::BORDERLESS);

	graphics::RenderManager r_mng;
	r_mng.Initialize(wnd);

	UINT index = r_mng.CreateShaderProgram("VertexShader.cso", "PixelShader.cso", 0);
	EXPECT_EQ(index, 0);

	r_mng.Destroy();
}

TEST(Graphics, CreateRenderProgramFaultyFilepath)
{
	UINT
		clientWidth = graphics::window::GetWorkAreaResolution().x,
		clientHeight = graphics::window::GetWorkAreaResolution().y;

	graphics::Window wnd;
	wnd.Initialize(
		clientWidth,
		clientHeight,
		"CreateRenderProgramFaultyFilepath",
		graphics::WINDOW_STYLE::BORDERLESS);

	graphics::RenderManager r_mng;
	r_mng.Initialize(wnd);

	UINT index = r_mng.CreateShaderProgram("VS_FaultyFilepath.cso", "PS_FaultyFilepath.cso", 0);
	EXPECT_EQ(index, UINT_MAX);

	r_mng.Destroy();
}

TEST(Graphics, CreateComputeProgramCorrectFilepath)
{
	graphics::ComputeManager c_mng;
	c_mng.Initialize();

	UINT index = c_mng.CreateShaderProgram("ComputeShader.cso");
	EXPECT_EQ(index, 0);

	c_mng.Destroy();
}

TEST(Graphics, CreateComputeProgramFaultyFilepath)
{
	graphics::ComputeManager c_mng;
	c_mng.Initialize();

	UINT index = c_mng.CreateShaderProgram("CS_FaultyFilepath.cso");
	EXPECT_EQ(index, UINT_MAX);

	c_mng.Destroy();
}

TEST(Graphics, UploadMeshesToMeshManager)
{
	graphics::MeshManager m_mng;
	m_mng.Initialize(999, 999);

	struct float3
	{
		float x, y, z;
	};

	float3 triangle[3] = {
		 0.0f, -1.0f, 0.1f,
		 0.5f,  0.0f, 0.1f,
		 1.0f, -1.0f, 0.1f,
	};

	float3 quad[6] = {
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
	};

	int quad_indices[6] = {
		3, 4, 5,
		0, 1, 2,
	};

	graphics::MeshRegion meshIndex0 = m_mng.CreateMeshRegion(3, 0);
	EXPECT_NE(meshIndex0.VertexRegion.Size, 0);
	EXPECT_EQ(meshIndex0.IndexRegion.Size, 0);
	{
		graphics::VERTEX_DATA vd = { NULL };
		vd.pVertexPositions = triangle;
		HRESULT hr = m_mng.UploadData(meshIndex0, vd, NULL);
		EXPECT_EQ(hr, S_OK);
	}

	graphics::MeshRegion meshIndex1 = m_mng.CreateMeshRegion(6, 6);
	EXPECT_NE(meshIndex1.VertexRegion.Size, 0);
	EXPECT_NE(meshIndex1.IndexRegion.Size, 0);
	{
		graphics::VERTEX_DATA vd = { NULL };
		vd.pVertexPositions = quad;
		HRESULT hr = m_mng.UploadData(meshIndex1, vd, quad_indices);
		EXPECT_EQ(hr, S_OK);
	}

	m_mng.Destroy();
}

TEST(Graphics, UploadMeshesToMeshManagerNoSpace)
{
	graphics::MeshManager m_mng;
	m_mng.Initialize(0, 0);

	struct float3
	{
		float x, y, z;
	};

	float3 triangle[3] = {
		 0.0f, -1.0f, 0.1f,
		 0.5f,  0.0f, 0.1f,
		 1.0f, -1.0f, 0.1f,
	};

	float3 quad[6] = {
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
	};

	int quad_indices[6] = {
		3, 4, 5,
		0, 1, 2,
	};

	graphics::MeshRegion meshIndex0 = m_mng.CreateMeshRegion(3, 0);
	EXPECT_EQ(meshIndex0.VertexRegion.Size, 0);
	EXPECT_EQ(meshIndex0.IndexRegion.Size, 0);
	{
		graphics::VERTEX_DATA vd = { NULL };
		vd.pVertexPositions = triangle;
		HRESULT hr = m_mng.UploadData(meshIndex0, vd, NULL);
		EXPECT_EQ(hr, E_INVALIDARG);
	}

	graphics::MeshRegion meshIndex1 = m_mng.CreateMeshRegion(6, 6);
	EXPECT_EQ(meshIndex1.VertexRegion.Size, 0);
	EXPECT_EQ(meshIndex1.IndexRegion.Size, 0);
	{
		graphics::VERTEX_DATA vd = { NULL };
		vd.pVertexPositions = quad;
		HRESULT hr = m_mng.UploadData(meshIndex1, vd, quad_indices);
		EXPECT_EQ(hr, E_INVALIDARG);
	}

	m_mng.Destroy();
}

int main(int argc, char** argv)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	::testing::InitGoogleTest(&argc, argv);
	int testsFailed = RUN_ALL_TESTS();

	system("pause");

	return 0;
}