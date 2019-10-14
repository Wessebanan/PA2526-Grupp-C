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
		const UINT vertexCountCapacity,
		const UINT indexCountCapacity)
	{
		HRESULT hr;

		hr = graphics::internal::InitializeD3D11();

		graphics::internal::D3D11_DEVICE_HANDLE handle;
		graphics::internal::GetD3D11(&handle);
		m_pDevice4	= handle.pDevice4;
		m_pContext4 = handle.pDeviceContext4;

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
				m_pDevice4,
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

			hr = m_pDevice4->CreateBuffer(&desc, NULL, &m_pIndexBuffer);
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

	void MeshManager::EnableVertexBuffers()
	{
		m_pContext4->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_pContext4->VSSetShaderResources(0, VERTEX_BUFFER_COUNT, m_pVertexBufferViews);
		m_pContext4->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
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
		const MeshRegion& mesh,
		const VERTEX_DATA& vertexData,
		const void* pIndices)
	{
		m_UploadVertexData(0, sizeof(float) * 3, vertexData.pVertexPositions, mesh.VertexRegion);
		m_UploadVertexData(1, sizeof(float) * 3, vertexData.pVertexNormals, mesh.VertexRegion);
		m_UploadVertexData(2, sizeof(float) * 2, vertexData.pVertexTexCoords, mesh.VertexRegion);
		m_UploadVertexData(3, sizeof(float) * 3, vertexData.pVertexBlendWeights, mesh.VertexRegion);
		m_UploadVertexData(4, sizeof(int)   * 4, vertexData.pVertexBlendIndices, mesh.VertexRegion);

		m_UploadIndexData(pIndices, mesh.IndexRegion);
	}

	void MeshManager::m_UploadVertexData(
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

		m_pContext4->UpdateSubresource(
			m_pVertexBuffers[index],
			0,
			&box,
			pData,
			rMeshRegion.Size * stride,
			0);
	}

	void MeshManager::m_UploadIndexData(
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

		m_pContext4->UpdateSubresource(
			m_pIndexBuffer,
			0,
			&box,
			pData,
			rIndexRegion.Size * stride,
			0);
	}
}