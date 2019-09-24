#include "../includes/Mesh.h"

MeshHeap::MeshHeap()
{
	m_offsets[0] = 0;
	m_offsets[1] = 0;
	m_offsets[2] = 0;

	m_strides[0] = sizeof(float) * 3;	// Vertex float3(x, y, z)
	m_strides[1] = sizeof(float) * 3;	// Normal float3(x, y, z)
	m_strides[2] = sizeof(float) * 2;	// UV	  float2(x, y)

	m_topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_count = 0;

	m_pVerticesAt = m_vertices;
	m_pUVAt = m_uv;

	m_currentLocation = 0;
}

MeshHeap::~MeshHeap()
{
}

void MeshHeap::AppendVertexData(
	const UINT count, 
	const void* pVertices,
	const void* pNormals,
	const void* pUVs,
	MeshRegion* pRegion)
{
	memcpy(m_pVerticesAt, pVertices, count * sizeof(float) * 3);
	m_pVerticesAt += count * sizeof(float) * 3;

	if (pUVs)
	{
		memcpy(m_pUVAt, pUVs, count * sizeof(float) * 3);
		m_pUVAt += count * sizeof(float) * 2;
	}

	m_vertexCount[m_count] = count;

	pRegion->VertexCount = count;
	pRegion->VertexLocation = m_currentLocation;

	m_currentLocation += count;
	m_count++;
}

void MeshHeap::Initialize(ID3D11Device4* pDevice4, const UINT elements)
{
	D3D11_BUFFER_DESC descFloat3 = { 0 };
	descFloat3.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	descFloat3.ByteWidth = sizeof(float) * 3 * elements;
	descFloat3.CPUAccessFlags = 0;
	descFloat3.Usage = D3D11_USAGE_DEFAULT;

	pDevice4->CreateBuffer(&descFloat3, NULL, &m_pVertexBuffers[0]);
	pDevice4->CreateBuffer(&descFloat3, NULL, &m_pVertexBuffers[1]);

	D3D11_BUFFER_DESC descFloat2 = { 0 };
	descFloat2.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	descFloat2.ByteWidth = sizeof(float) * 2 * elements;
	descFloat2.CPUAccessFlags = 0;
	descFloat2.Usage = D3D11_USAGE_DEFAULT;

	pDevice4->CreateBuffer(&descFloat2, NULL, &m_pVertexBuffers[2]);
}

void MeshHeap::Release()
{
	m_pVertexBuffers[0]->Release();
	m_pVertexBuffers[1]->Release();
	m_pVertexBuffers[2]->Release();
}
