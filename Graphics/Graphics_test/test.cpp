#include "pch.h"

#include "../includes/Window.h"

#include "../includes/RenderContext.h"
#include "../includes/RenderManager.h"
#include "../includes/MeshManager.h"
#include "../includes/ComputeManager.h"

#include "../Application/ForwardRenderingPipeline.h"

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
	graphics::InitializeD3D11();

	graphics::RenderManager mng;
	HRESULT hr = mng.Initialize(65536);

	EXPECT_EQ(hr, S_OK);

	mng.Destroy();

	graphics::DestroyD3D11();
}

TEST(Graphics, MeshManager)
{
	graphics::InitializeD3D11();

	graphics::MeshManager mng;
	HRESULT hr = mng.Initialize(999, 999);

	EXPECT_EQ(hr, S_OK);

	mng.Destroy();
	graphics::DestroyD3D11();
}

TEST(Graphics, ComputeManager)
{
	graphics::InitializeD3D11();

	graphics::ComputeManager mng;
	HRESULT hr = mng.Initialize();

	EXPECT_EQ(hr, S_OK);

	mng.Destroy();
	graphics::DestroyD3D11();
}

TEST(Graphics, MultipleManagers)
{
	graphics::InitializeD3D11();

	HRESULT hr			= S_OK;

	graphics::RenderManager r_mng;
	hr = r_mng.Initialize(65536);

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

	graphics::DestroyD3D11();
}

TEST(Graphics, CreatePipeline)
{
	UINT
		clientWidth = graphics::GetWorkAreaResolution().x,
		clientHeight = graphics::GetWorkAreaResolution().y;

	graphics::Window wnd;
	wnd.Initialize(
		clientWidth,
		clientHeight,
		"CreatePipeline",
		graphics::WINDOW_STYLE::BORDERLESS);

	graphics::InitializeD3D11();
	graphics::AttachHwndToSwapChain(wnd);

	graphics::RenderManager r_mng;
	r_mng.Initialize(65536);

	graphics::FORWARD_RENDERING_PIPELINE_DESC desc = { 0 };
	desc.NearPlane	= 0.1f;
	desc.FarPlane	= 1.0f;
	desc.ClientHeight = 100;
	desc.ClientWidth = 100;
	desc.Fov = 3.14f;

	UINT index = r_mng.CreatePipeline(
		new graphics::ForwardRenderingPipeline(),
		&desc);

	EXPECT_EQ(index, 0);

	r_mng.Destroy();
	graphics::DestroyD3D11();
}

TEST(Graphics, CreateRenderProgramCorrectFilepath)
{
	graphics::InitializeD3D11();

	graphics::RenderManager r_mng;
	r_mng.Initialize(65536);

	UINT index = r_mng.CreateShaderProgram("VertexShader.cso", "PixelShader.cso", 0);
	EXPECT_EQ(index, 0);

	r_mng.Destroy();
	graphics::DestroyD3D11();
}

TEST(Graphics, CreateRenderProgramFaultyFilepath)
{
	graphics::InitializeD3D11();

	graphics::RenderManager r_mng;
	r_mng.Initialize(65536);

	UINT index = r_mng.CreateShaderProgram("VS_FaultyFilepath.cso", "PS_FaultyFilepath.cso", 0);
	EXPECT_EQ(index, UINT_MAX);

	r_mng.Destroy();

	graphics::DestroyD3D11();
}

TEST(Graphics, CreateComputeProgramCorrectFilepath)
{
	graphics::InitializeD3D11();

	graphics::ComputeManager c_mng;
	c_mng.Initialize();

	UINT index = c_mng.CreateShaderProgram("ComputeShader.cso");
	EXPECT_EQ(index, 0);

	c_mng.Destroy();

	graphics::DestroyD3D11();
}

TEST(Graphics, CreateComputeProgramFaultyFilepath)
{
	graphics::InitializeD3D11();

	graphics::ComputeManager c_mng;
	c_mng.Initialize();

	UINT index = c_mng.CreateShaderProgram("CS_FaultyFilepath.cso");
	EXPECT_EQ(index, UINT_MAX);

	c_mng.Destroy();

	graphics::DestroyD3D11();
}

TEST(Graphics, UploadMeshesToMeshManager)
{
	graphics::InitializeD3D11();

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
	graphics::DestroyD3D11();
}

TEST(Graphics, UploadMeshesToMeshManagerNoSpace)
{
	graphics::InitializeD3D11();

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

	graphics::DestroyD3D11();
}

int main(int argc, char** argv)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	::testing::InitGoogleTest(&argc, argv);
	int testsFailed = RUN_ALL_TESTS();

	system("pause");

	return 0;
}