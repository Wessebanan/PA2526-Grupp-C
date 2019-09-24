#pragma once

#include <string>
#include <DirectXMath.h>

#include "..//includes/Renderer.h"

#ifdef _DEBUG
#pragma comment(lib, "GraphicsEngine_d.lib")
#else
#pragma comment(lib, "GraphicsEngine.lib")
#endif // DEBUG

TEST(TestingRenderer, CreatingDefaultMesh)
{
	struct float3
	{
		float x, y, z;
	};

	float3 vertexData[3] = {
		-1.0f, -1.0f, 0.0f,
		 0.0f,  1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
	};

	graphics::Renderer renderer;
	renderer.Initialize(800, 600, "Testing renderer");

	graphics::VERTEX_BUFFER_DATA vb = { 0 };
	vb.VertexCount = 3;
	vb.pVertexData = vertexData;

	graphics::GPUMesh mesh;
	int result = renderer.CreateMesh(&vb, NULL, mesh);
	
	EXPECT_TRUE(result);
	EXPECT_GE(mesh.BufferIndex, 0);
	EXPECT_FALSE(mesh.IsIndexed);

	renderer.Release();
}

TEST(TestingRenderer, CreatingIndexedMesh)
{
	struct float3
	{
		float x, y, z;
	};

	float3 vertexData[4] = {
		-1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,
	};

	int indices[6] = {
		0, 1, 2,
		1, 2, 3
	};

	graphics::Renderer renderer;
	renderer.Initialize(800, 600, "Testing renderer");

	graphics::VERTEX_BUFFER_DATA vb = { 0 };
	vb.VertexCount = 4;
	vb.pVertexData = vertexData;

	graphics::INDEX_BUFFER_DATA ib;
	ib.IndexCount = 6;
	ib.pIndexData = indices;

	graphics::GPUMesh mesh;
	int result = renderer.CreateMesh(&vb, &ib, mesh);

	EXPECT_TRUE(result);
	EXPECT_GE(mesh.BufferIndex, 0);
	EXPECT_TRUE(mesh.IsIndexed);

	renderer.Release();
}
