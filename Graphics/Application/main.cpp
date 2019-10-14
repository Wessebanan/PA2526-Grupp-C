
#include "../includes/GraphicsInterface.h"
#include "../includes/Window.h"
#include "../includes/RenderManager.h"
#include "../includes/ForwardRenderingPipeline.h"

#include <memory>
#include <DirectXMath.h>
#include <vector>

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

void SetViewMatrix(
	DirectX::XMFLOAT4X4& rViewMatrix,
	const float x,
	const float y,
	const float z)
{
	DirectX::XMStoreFloat4x4(&rViewMatrix,
		DirectX::XMMatrixLookToLH(
			{ x, y, z },
			{ 0.0f, 0.0f,  1.0f },
			{ 0.0f, 1.0f,  0.0f }
	));
}

void SetProjectionMatrix(
	DirectX::XMFLOAT4X4& rProjectionMatrix,
	const float fov,
	const float aspectRatio,
	const float nearPlane,
	const float farPlane)
{
	DirectX::XMStoreFloat4x4(&rProjectionMatrix,
		DirectX::XMMatrixPerspectiveFovLH(
			fov, aspectRatio, nearPlane, farPlane
		));
}

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	UINT 
		clientWidth = 1600, 
		clientHeight = 900;

	graphics::Window wnd;
	wnd.Initialize(
		clientWidth,
		clientHeight,
		"Couch Commanders",
		graphics::WINDOW_STYLE::BORDER);

	graphics::RenderManager mng;
	mng.Initialize(clientWidth, clientHeight, wnd);

	UINT pipelineIndex0;
	{
		graphics::FORWARD_RENDERING_PIPELINE_DESC desc;
		desc.ClientWidth	= clientWidth;
		desc.ClientHeight	= clientHeight;
		desc.Fov			= 3.14f / 2.0f;
		desc.NearPlane		= 0.1f;
		desc.FarPlane		= 100.0f;
		pipelineIndex0		= mng.CreateGraphicsPipeline(
			new graphics::ForwardRenderingPipeline, 
			&desc);
	}

	UINT shaderIndex0 = mng.CreateGraphicsShaderProgram(
		"VertexShader.cso", 
		"PixelShader.cso", 
		sizeof(float) * 4);

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

	graphics::MeshRegion meshIndex0 = mng.CreateMeshRegion(3, 0);
	{
		graphics::VERTEX_DATA vd = { NULL };
		vd.pVertexPositions = triangle;
		mng.UploadMeshData(meshIndex0, vd, NULL);
	}

	graphics::MeshRegion meshIndex1 = mng.CreateMeshRegion(6, 6);
	{
		graphics::VERTEX_DATA vd = { NULL };
		vd.pVertexPositions = quad;
		mng.UploadMeshData(meshIndex1, vd, quad_indices);
	}

	graphics::ShaderModelLayout layout;
	layout.Meshes[0] = (meshIndex0);
	layout.Meshes[1] = (meshIndex1);

	layout.InstanceCounts[0] = (2);
	layout.InstanceCounts[1] = (2);

	layout.MeshCount	= 2;
	layout.TotalModels	= 4;

	struct float4
	{
		float x, y, z, w;
	};

	float4 pos[4] = {
		{  0.0f,  0.0f, 0.0f, 0.0f },
		{ -1.0f,  0.0f, 0.0f, 0.0f },
		{  0.0f,  0.0f, 0.0f, 0.0f },
		{ -1.0f,  0.0f, 0.0f, 0.0f },
	};

	mng.SetModelData(pos, sizeof(pos));
	mng.SetShaderModelLayout(shaderIndex0, layout);

	float x = 0.0f, y = 0.0f, z = -1.0f;
	DirectX::XMFLOAT4X4 m_cameraMatrix;

	wnd.Open();
	while (wnd.IsOpen())
	{
		if (!wnd.Update())
		{
			float moveSpeed = 0.1f;
			if (GetAsyncKeyState(VK_UP))
			{
				z += moveSpeed;
			}
			if (GetAsyncKeyState(VK_DOWN))
			{
				z -= moveSpeed;
			}
			if (GetAsyncKeyState(VK_ESCAPE))
			{
				wnd.Close();
			}

			SetViewMatrix(m_cameraMatrix, x, y, z);

			{
				graphics::FORWARD_RENDERING_PIPELINE_DATA data;
				data.ViewMatrix = m_cameraMatrix;

				mng.UpdateGraphicsPipelineData(pipelineIndex0, &data);
			} 

			mng.ExecuteGraphicsPipeline(pipelineIndex0);
			mng.Present();
		}
	}

	mng.Destroy();

	return 0;
}