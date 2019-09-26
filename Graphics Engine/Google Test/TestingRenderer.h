#pragma once
//
//#include <string>
//#include <DirectXMath.h>
//
//#include "..//includes/rendering/Renderer.h"
//
//#ifdef _DEBUG
//#pragma comment(lib, "GraphicsEngine_d.lib")
//#else
//#pragma comment(lib, "GraphicsEngine.lib")
//#endif // DEBUG
//
//TEST(TestingRenderer, CreatingDefaultMesh)
//{
//	// create vertex data
//	struct float3
//	{
//		float x, y, z;
//	};
//
//	float3 vertex_data[3] = {
//		-1.0f, -1.0f, 0.0f,
//		 0.0f,  1.0f, 0.0f,
//		 1.0f, -1.0f, 0.0f,
//	};
//
//	graphics::Renderer renderer;
//	renderer.Initialize(800, 600, "Testing renderer");
//
//	// assign vertex data
//	graphics::VERTEX_BUFFER_DATA vb = { 0 };
//	vb.VertexCount = 3;
//	vb.pVertexData = vertex_data;
//
//	// create mesh with specified data
//	graphics::GPUMesh mesh;
//	int result = renderer.CreateMesh(&vb, NULL, mesh);
//	
//	// check return values
//	EXPECT_TRUE(result);
//	EXPECT_GE(mesh.BufferIndex, 0);
//	EXPECT_FALSE(mesh.IsIndexed);
//
//	renderer.Release();
//}
//
//TEST(TestingRenderer, CreatingIndexedMesh)
//{
//	// Create vertex and index data
//	struct float3
//	{
//		float x, y, z;
//	};
//
//	float3 vertex_data[4] = {
//		-1.0f, -1.0f, 0.0f,
//		-1.0f,  1.0f, 0.0f,
//		 1.0f, -1.0f, 0.0f,
//		 1.0f,  1.0f, 0.0f,
//	};
//
//	int indices[6] = {
//		0, 1, 2,
//		1, 2, 3
//	};
//
//	graphics::Renderer renderer;
//	renderer.Initialize(800, 600, "Testing renderer");
//
//	// Assign vertex data
//	graphics::VERTEX_BUFFER_DATA vb = { 0 };
//	vb.VertexCount = 4;
//	vb.pVertexData = vertex_data;
//
//	// Assign index data
//	graphics::INDEX_BUFFER_DATA ib;
//	ib.IndexCount = 6;
//	ib.pIndexData = indices;
//
//	// Create mesh with specified data
//	graphics::GPUMesh mesh;
//	int result = renderer.CreateMesh(&vb, &ib, mesh);
//
//	// check return values
//	EXPECT_TRUE(result);
//	EXPECT_GE(mesh.BufferIndex, 0);
//	EXPECT_TRUE(mesh.IsIndexed);
//
//	renderer.Release();
//}
