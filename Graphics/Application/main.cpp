
#include "../includes/GraphicsInterface.h"
#include "../includes/Window.h"

#include "../includes/MeshManager.h"
#include "../includes/RenderManager.h"
#include "../includes/ComputeManager.h"

#include "ForwardRenderingPipeline.h"

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
		clientWidth		= graphics::window::GetWorkAreaResolution().x,
		clientHeight	= graphics::window::GetWorkAreaResolution().y;

	graphics::Window wnd;
	wnd.Initialize(
		clientWidth,
		clientHeight,
		"Couch Commanders",
		graphics::WINDOW_STYLE::BORDERLESS);

	graphics::InitializeD3D11();
	graphics::AttachHwndToSwapChain(wnd);

	graphics::RenderManager r_mng;
	r_mng.Initialize(65536);

	graphics::MeshManager m_mng;
	m_mng.Initialize(999, 999);

	graphics::ComputeManager c_mng;
	c_mng.Initialize();


	UINT pipelineIndex0;
	{
		graphics::FORWARD_RENDERING_PIPELINE_DESC desc;
		desc.ClientWidth	= clientWidth;
		desc.ClientHeight	= clientHeight;
		desc.Fov			= 3.14f / 2.0f;
		desc.NearPlane		= 0.1f;
		desc.FarPlane		= 100.0f;
		pipelineIndex0		= r_mng.CreatePipeline(
			new graphics::ForwardRenderingPipeline, 
			&desc);
	}

	struct float4
	{
		float x, y, z, w;
	};

	UINT shaderIndex0 = r_mng.CreateShaderProgram(
		"VS_Default.cso", 
		"PS_Default.cso", 
		sizeof(float4));

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

	graphics::MeshRegion mesh_regions[2];
	UINT instance_counts[2] = { 2, 2 };

	mesh_regions[0] = m_mng.CreateMeshRegion(3, 0);
	{
		graphics::VERTEX_DATA vd = { NULL };
		vd.pVertexPositions = triangle;
		m_mng.UploadData(mesh_regions[0], vd, NULL);
	}

	mesh_regions[1] = m_mng.CreateMeshRegion(6, 6);
	{
		graphics::VERTEX_DATA vd = { NULL };
		vd.pVertexPositions = quad;
		m_mng.UploadData(mesh_regions[1], vd, quad_indices);
	}

	graphics::ShaderModelLayout layout;
	layout.MeshCount				= 2;
	layout.pMeshes					= mesh_regions;
	layout.pInstanceCountPerMesh	= instance_counts;

	float4 pos[4] = {
		{  0.0f,  0.0f, 0.0f, 0.0f },
		{ -1.0f,  0.0f, 0.0f, 0.0f },
		{  0.0f,  0.0f, 0.0f, 0.0f },
		{ -1.0f,  0.0f, 0.0f, 0.0f },
	};

	r_mng.BeginUpload();
	r_mng.UploadPerInstanceData(pos, sizeof(float4) * 2, 0);
	r_mng.UploadPerInstanceData(pos, sizeof(float4) * 2, sizeof(float4) * 2);
	r_mng.SetShaderModelLayout(shaderIndex0, layout);

	float x = 0.0f, y = 0.0f, z = -1.0f;
	DirectX::XMFLOAT4X4 m_cameraMatrix;

	wnd.Open();
	while (wnd.IsOpen())
	{
		if (!wnd.Update() && graphics::window::IsInFocus(wnd))
		{
			float moveSpeed = 0.001f;
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

				r_mng.UpdatePipeline(pipelineIndex0, &data);
			} 

			m_mng.SetVertexBuffers();
			r_mng.ExecutePipeline(pipelineIndex0);

			graphics::Present(0);
		}
	}

	c_mng.Destroy();
	m_mng.Destroy();
	r_mng.Destroy();

	graphics::DestroyD3D11();

	return 0;
}