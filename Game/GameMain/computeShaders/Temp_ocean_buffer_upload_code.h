#pragma once

//#include <d3d11.h>

#include <DirectXMath.h>

#include "Mesh.h"

using namespace DirectX;

struct TileMeshConstBuf
{
	int vertex_count;
	float vertices[60*4];
};

struct LookUpTableConstBuf
{
	unsigned int oceanTileOffset;
	unsigned int oceanTileCount;
	float* xzPositions;
};

void UploadTileMeshToGPU(ModelLoader::Mesh* pTileMesh, TileMeshConstBuf** pMeshBufferStruct)
{
	std::vector<XMFLOAT3>* p_mesh_vertices = pTileMesh->GetVertexPositionVector();
	std::vector<int>* p_mesh_indices = pTileMesh->GetIndexVector();

	TileMeshConstBuf* p_mesh_buffer = new TileMeshConstBuf;

	// -- Scale matrix
	const unsigned int OCEAN_SCALE_RATIO = 1.1f;
	XMMATRIX scale_matrix = XMMatrixScaling(OCEAN_SCALE_RATIO, OCEAN_SCALE_RATIO, OCEAN_SCALE_RATIO);

	int float_counter = 0;
	XMFLOAT3 vertex;
	XMVECTOR xm_vertex;
	for (int index : *p_mesh_indices)
	{
		vertex = (*p_mesh_vertices)[index];
		
		xm_vertex = XMLoadFloat3(&vertex);
		xm_vertex = XMVector3Transform(xm_vertex, scale_matrix);
		XMStoreFloat3(&vertex, xm_vertex);


		p_mesh_buffer->vertices[float_counter]		= vertex.x;
		p_mesh_buffer->vertices[float_counter + 1] = vertex.y;
		p_mesh_buffer->vertices[float_counter + 2] = vertex.z;
		p_mesh_buffer->vertices[float_counter + 3] = 1.0f;

		float_counter += 4;
	}

	// TODO: Check so that vertex_count really is 60..
	p_mesh_buffer->vertex_count = 60;

	// AVOID MEMORY LEAKS BEFORE
	//delete 
}