
#include "../includes/MeshManager.h"

namespace graphics
{
	MeshManager::MeshManager()
	{
		m_sizeV		= 0;
		m_capacityV = 0;

		m_sizeI		= 0;
		m_capacityI = 0;

		m_pIndexBuffer = NULL;

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

		if (vertexCountCapacity == 0 || indexCountCapacity == 0) return E_INVALIDARG;

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

			if (FAILED(hr)) return hr;
		}

		hr = internal::CreateAndSetVertexBuffers(
			m_pDevice4, 
			m_pContext4, 
			vertexCountCapacity);

		if (FAILED(hr)) return hr;

		m_sizeV		= 0;
		m_capacityV = vertexCountCapacity;

		m_sizeI		= 0;
		m_capacityI = indexCountCapacity;

		return S_OK;
	}

	graphics::MeshRegion MeshManager::CreateMeshRegion(
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

	void MeshManager::SetVertexBuffers()
	{
		m_pContext4->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_pContext4->VSSetShaderResources(0, VERTEX_BUFFER_COUNT, m_pVertexBufferViews);
		m_pContext4->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	}

	void MeshManager::Clear()
	{
		m_sizeV = 0;
		m_sizeI = 0;
	}

	void MeshManager::Destroy()
	{
		for (UINT i = 0; i < VERTEX_BUFFER_COUNT; i++)
		{
			graphics::SafeRelease((IUnknown**)&m_pVertexBuffers[i]);
			graphics::SafeRelease((IUnknown**)&m_pVertexBufferViews[i]);
		}

		graphics::SafeRelease((IUnknown**)&m_pIndexBuffer);
	}

	HRESULT MeshManager::UploadData(
		const MeshRegion& mesh,
		const VERTEX_DATA& vertexData,
		const void* pIndices)
	{
		HRESULT hr = S_OK;
		if (mesh.VertexRegion.Size == 0) return E_INVALIDARG;

		hr = m_UploadVertexData(0, sizeof(float) * 3, vertexData.pVertexPositions, mesh.VertexRegion);
		if (FAILED(hr)) return hr;
		
		hr = m_UploadVertexData(1, sizeof(float) * 3, vertexData.pVertexNormals, mesh.VertexRegion);
		if (FAILED(hr)) return hr;

		hr = m_UploadVertexData(2, sizeof(float) * 2, vertexData.pVertexTexCoords, mesh.VertexRegion);
		if (FAILED(hr)) return hr;
		
		hr = m_UploadVertexData(3, sizeof(float) * 3, vertexData.pVertexBlendWeights, mesh.VertexRegion);
		if (FAILED(hr)) return hr;
		
		hr = m_UploadVertexData(4, sizeof(int)   * 4, vertexData.pVertexBlendIndices, mesh.VertexRegion);
		if (FAILED(hr)) return hr;

		if (pIndices)
		{
			if (mesh.IndexRegion.Size == 0) return E_INVALIDARG;

			hr = m_UploadIndexData(pIndices, mesh.IndexRegion);
			return hr;
		}
		else
		{
			return S_OK;
		}
	}

	HRESULT MeshManager::m_UploadVertexData(
		const UINT index, 
		const UINT stride, 
		const void* pData,
		const BufferRegion& rMeshRegion)
	{
		if (!pData) return S_FALSE;

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

		return S_OK;
	}

	HRESULT MeshManager::m_UploadIndexData(
		const void* pData, 
		const BufferRegion& rIndexRegion)
	{
		if (!pData) return S_FALSE;

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

		return S_OK;
	}
}