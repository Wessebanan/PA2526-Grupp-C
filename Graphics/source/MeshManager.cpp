#include "../includes/MeshManager.h"

namespace graphics
{
	MeshManager::MeshManager()
	{
		m_sizeV		= 0;
		m_capacityV = 0;

		m_sizeI		= 0;
		m_capacityI = 0;

		ZeroMemory(m_pVertexBuffers, sizeof(m_pVertexBuffers));
		ZeroMemory(m_pVertexBufferViews, sizeof(m_pVertexBufferViews));
	}

	MeshManager::~MeshManager()
	{
	}

	HRESULT MeshManager::Initialize(
		ID3D11Device4* pDevice4,
		const UINT vertexCountCapacity,
		const UINT indexCountCapacity)
	{
		HRESULT hr;

		UINT strides[VERTEX_BUFFER_COUNT] =
		{
			sizeof(float) * 3,					// Vertex Positions
			sizeof(float) * 3,					// Vertex Normals
			sizeof(float) * 2,					// Vertex Tex Coord

			sizeof(float) * 3,					// Vertex Blend Weight
			sizeof(int)	 * 4,					// Vertex Blend Indices
		};

		for (UINT i = 0; i < VERTEX_BUFFER_COUNT; i++)
		{
			hr = graphics::CreateVertexBuffer(
				pDevice4,
				NULL,
				vertexCountCapacity * strides[i],
				strides[i],
				0,
				D3D11_USAGE_DEFAULT,
				&m_pVertexBuffers[i],
				&m_pVertexBufferViews[i]);

			if (FAILED(hr)) return hr;
		}

		{
			D3D11_BUFFER_DESC desc = { 0 };
			desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			desc.ByteWidth = indexCountCapacity * sizeof(int);
			desc.CPUAccessFlags = 0;
			desc.Usage = D3D11_USAGE_DEFAULT;

			hr = pDevice4->CreateBuffer(&desc, NULL, &m_pIndexBuffer);
		}

		if (FAILED(hr)) return hr;

		m_sizeV		= 0;
		m_capacityV = vertexCountCapacity;

		m_sizeI		= 0;
		m_capacityI = indexCountCapacity;

		return S_OK;
	}

	MeshRegion MeshManager::CreateMeshRegion(
		const UINT vertexCount, 
		const UINT indexCount)
	{
		MeshRegion mesh;
		mesh.VertexRegion	= { 0, 0 };
		mesh.IndexRegion	= { 0, 0 };

		if (m_sizeV + vertexCount > m_capacityV) return mesh;
		if (m_sizeI + indexCount > m_capacityI) return mesh;

		mesh.VertexRegion	= { m_sizeV, vertexCount };
		mesh.IndexRegion	= { m_sizeI, indexCount };

		m_sizeV += vertexCount;
		m_sizeI += indexCount;

		return mesh;
	}

	void MeshManager::EnableVertexBuffers(ID3D11DeviceContext4* pContext4, const UINT startSlot)
	{
		pContext4->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pContext4->VSSetShaderResources(startSlot, VERTEX_BUFFER_COUNT, m_pVertexBufferViews);
		pContext4->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	}

	void MeshManager::Destroy()
	{
		for (UINT i = 0; i < VERTEX_BUFFER_COUNT; i++)
		{
			m_pVertexBuffers[i]->Release();
			m_pVertexBufferViews[i]->Release();
		}

		graphics::SafeRelease((IUnknown**)&m_pIndexBuffer);
	}

	void MeshManager::UploadData(
		ID3D11DeviceContext4* pContext4, 
		const MeshRegion& mesh,
		const VERTEX_DATA& vertexData,
		const void* pIndices)
	{
		m_UploadVertexData(pContext4, 0, sizeof(float) * 3, vertexData.pVertexPositions, mesh.VertexRegion);
		m_UploadVertexData(pContext4, 1, sizeof(float) * 3, vertexData.pVertexNormals, mesh.VertexRegion);
		m_UploadVertexData(pContext4, 2, sizeof(float) * 2, vertexData.pVertexTexCoords, mesh.VertexRegion);
		m_UploadVertexData(pContext4, 3, sizeof(float) * 3, vertexData.pVertexBlendWeights, mesh.VertexRegion);
		m_UploadVertexData(pContext4, 4, sizeof(int)   * 4, vertexData.pVertexBlendIndices, mesh.VertexRegion);

		m_UploadIndexData(pContext4, pIndices, mesh.IndexRegion);
	}

	void MeshManager::m_UploadVertexData(
		ID3D11DeviceContext4* pContext4,
		const UINT index, 
		const UINT stride, 
		const void* pData,
		const BufferRegion& rMeshRegion)
	{
		if (!pData || rMeshRegion.Size < 1) return;

		D3D11_BOX box;
		box.back	= 1;
		box.bottom	= 1;
		box.top		= 0;
		box.front	= 0;

		box.left	= (rMeshRegion.Location * stride);
		box.right	= (rMeshRegion.Location + rMeshRegion.Size) * stride;

		pContext4->UpdateSubresource(
			m_pVertexBuffers[index],
			0,
			&box,
			pData,
			rMeshRegion.Size * stride,
			0);
	}

	void MeshManager::m_UploadIndexData(
		ID3D11DeviceContext4* pContext4, 
		const void* pData, 
		const BufferRegion& rIndexRegion)
	{
		if (!pData || rIndexRegion.Size < 1) return;

		const UINT stride = sizeof(int);

		D3D11_BOX box;
		box.back	= 1;
		box.bottom	= 1;
		box.top		= 0;
		box.front	= 0;

		box.left	= (rIndexRegion.Location * stride);
		box.right	= (rIndexRegion.Location + rIndexRegion.Size) * stride;

		pContext4->UpdateSubresource(
			m_pIndexBuffer,
			0,
			&box,
			pData,
			rIndexRegion.Size * stride,
			0);
	}
}