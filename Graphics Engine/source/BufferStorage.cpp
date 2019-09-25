#include "../includes/pch.h"
#include "../includes/BufferStorage.h"

namespace graphics
{

#define BUFFER_VERTEX_ARRAY_INDEX_LENGTH (1024)

	BufferHeap::BufferHeap()
	{
		// DEFAULT VALUES (WILL BE OVERWRITTEN)
		m_currentSpent		= 0;
		m_size				= 0;
		m_pBuffer			= NULL;
		m_type				= BUFFER_CONSTANT_DYNAMIC;
		m_bindType			= D3D11_BIND_CONSTANT_BUFFER;
		m_currentCreated	= 0;
		m_pCpuData			= NULL;

		//ZeroMemory(m_cpuData, ARRAYSIZE(m_cpuData));
	}

	BufferHeap::~BufferHeap()
	{
	}

	int BufferHeap::Initialize(
		ID3D11Device4* pDevice4,
		BUFFER_TYPE type,
		char** pDataStart)
	{
		m_type = type;

		D3D11_BUFFER_DESC desc = { 0 };
		desc.CPUAccessFlags = 0;

		switch (type)
		{
		case BUFFER_CONSTANT_DYNAMIC:
			desc.BindFlags	= m_bindType = D3D11_BIND_CONSTANT_BUFFER;
			desc.ByteWidth	= ALLOCATED_BUFFER_SIZE;
			desc.Usage		= D3D11_USAGE_DYNAMIC;
			desc.MiscFlags	= 0;

			m_size = ALLOCATED_BUFFER_SIZE;
			break;

		case BUFFER_CONSTANT_STATIC:
			desc.BindFlags	= m_bindType = D3D11_BIND_CONSTANT_BUFFER;
			desc.ByteWidth	= ALLOCATED_BUFFER_SIZE;
			desc.Usage		= D3D11_USAGE_DEFAULT;
			desc.MiscFlags	= 0;

			m_size = ALLOCATED_BUFFER_SIZE;
			break;

		case BUFFER_RENDER_TARGET:
			desc.BindFlags	= m_bindType = D3D11_BIND_RENDER_TARGET;
			desc.ByteWidth	= ALLOCATED_RTV_AND_SRV_SIZE;
			desc.Usage		= D3D11_USAGE_DEFAULT;
			desc.MiscFlags	= 0;

			m_size = ALLOCATED_RTV_AND_SRV_SIZE;
			break;

		case BUFFER_SHADER_RESOURCE:
			desc.BindFlags	= m_bindType = D3D11_BIND_SHADER_RESOURCE;
			desc.ByteWidth	= ALLOCATED_RTV_AND_SRV_SIZE;
			desc.Usage		= D3D11_USAGE_DEFAULT;
			desc.MiscFlags	= 0;

			m_size = ALLOCATED_RTV_AND_SRV_SIZE;
			break;

		case BUFFER_VERTEX_INDICES:
			desc.BindFlags	= m_bindType = D3D11_BIND_INDEX_BUFFER;
			desc.ByteWidth	= ALLOCATED_BUFFER_SIZE;
			desc.Usage		= D3D11_USAGE_DEFAULT;
			desc.MiscFlags	= 0;

			m_size = ALLOCATED_BUFFER_SIZE;
			break;

		case BUFFER_VERTEX_POSITION:
			desc.BindFlags	= m_bindType = D3D11_BIND_VERTEX_BUFFER;
			desc.ByteWidth	= ALLOCATED_BUFFER_SIZE;
			desc.Usage		= D3D11_USAGE_DEFAULT;
			desc.MiscFlags	= 0;

			m_size = ALLOCATED_BUFFER_SIZE;
			break;

		case BUFFER_VERTEX_NORMAL:
			desc.BindFlags	= m_bindType = D3D11_BIND_VERTEX_BUFFER;
			desc.ByteWidth	= ALLOCATED_BUFFER_SIZE;
			desc.Usage		= D3D11_USAGE_DEFAULT;
			desc.MiscFlags	= 0;

			m_size = ALLOCATED_BUFFER_SIZE;
			break;

		case BUFFER_VERTEX_UV:
			desc.BindFlags	= m_bindType = D3D11_BIND_VERTEX_BUFFER;
			desc.ByteWidth	= ALLOCATED_BUFFER_SIZE;
			desc.Usage		= D3D11_USAGE_DEFAULT;
			desc.MiscFlags	= 0;

			m_size = ALLOCATED_BUFFER_SIZE;
			break;

		case BUFFER_VERTEX_ARRAY_INDEX:
			desc.BindFlags	= m_bindType = D3D11_BIND_VERTEX_BUFFER;
			desc.ByteWidth	= BUFFER_VERTEX_ARRAY_INDEX_LENGTH * sizeof(UINT);
			desc.Usage		= D3D11_USAGE_IMMUTABLE;
			desc.MiscFlags	= 0;

			m_size = BUFFER_VERTEX_ARRAY_INDEX_LENGTH * sizeof(UINT);
			break;

		default:
			break;
		}

		if (desc.Usage == D3D11_USAGE_DYNAMIC)
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;


		constexpr UINT elements = BUFFER_VERTEX_ARRAY_INDEX_LENGTH;
		UINT indices[elements];
		D3D11_SUBRESOURCE_DATA data = { 0 };
		HRESULT hr;

		switch (type)
		{
		case BUFFER_VERTEX_ARRAY_INDEX:
			for (UINT i = 0; i < elements; i++)
			{
				indices[i] = i;
			}

			data.pSysMem = indices;

			hr = pDevice4->CreateBuffer(&desc, &data, &m_pBuffer);
			if (FAILED(hr)) return FALSE;
			break;

		default:
			hr = pDevice4->CreateBuffer(&desc, NULL, &m_pBuffer);
			if (FAILED(hr)) return FALSE;
			break;
		}

		m_pCpuData = *pDataStart;
		*pDataStart += m_size;
		return TRUE;
	}

	int BufferHeap::AllocateRegion(const UINT size, BufferRegion* pRegion)
	{
		UINT allocatedSize = size;

		// Constant buffers need alignment because of Direct3D 
		if (m_bindType == D3D11_BIND_CONSTANT_BUFFER)
		{
			allocatedSize = (UINT)ceil(allocatedSize / 256.0f);
			allocatedSize *= 256;
		}

		if (allocatedSize + m_currentSpent > m_size) return FALSE;

		pRegion->DataLocation	= m_currentSpent;
		pRegion->Type			= m_type;
		pRegion->DataCount		= allocatedSize;
		pRegion->ID				= m_currentCreated++;

		m_currentSpent		+= allocatedSize;

		return TRUE;
	}

	void BufferHeap::CopyDataToRegion(const void* pData, const BufferRegion& region)
	{
		switch (region.Type)
		{
		case BUFFER_VERTEX_ARRAY_INDEX:
			// DON'T COPY ANYTHING
			// HAS ALL THE DATA IT NEEDS
			break;

		default:
			memcpy(
				m_pCpuData + region.DataLocation,
				pData,
				region.DataCount);
			break;
		}
	}

	void BufferHeap::UploadToGPU(
		ID3D11DeviceContext4* pContext4,
		const BUFFER_TYPE type)
	{
		switch (type)
		{
		case BUFFER_CONSTANT_DYNAMIC:
			D3D11_MAPPED_SUBRESOURCE map;
			pContext4->Map(
				m_pBuffer,
				0,
				D3D11_MAP_WRITE_DISCARD,
				0,
				&map);

			memcpy(
				(char*)map.pData,
				m_pCpuData,
				m_currentSpent);

			pContext4->Unmap(m_pBuffer, 0);
			break;

		case BUFFER_CONSTANT_STATIC:
			pContext4->UpdateSubresource(
				m_pBuffer,
				0,
				NULL,
				m_pCpuData,
				m_currentSpent,
				0);
			break;

		case BUFFER_VERTEX_ARRAY_INDEX:
			// DON'T UPLOAD ANYTHING
			// HAS ALL THE DATA IT NEEDS
			break;

		default:
			D3D11_BOX box;
			box.back = 1;
			box.bottom = 1;
			box.top = 0;
			box.front = 0;

			box.left = 0;
			box.right = m_currentSpent;

			pContext4->UpdateSubresource(
				m_pBuffer,
				0,
				&box,
				m_pCpuData,
				m_currentSpent,
				0);
			break;
		}
	}

	void BufferHeap::Release()
	{
		m_pBuffer->Release();
	}

	InternalStorage::InternalStorage()
	{
		ZeroMemory(m_cpuData, sizeof(m_cpuData));
	}

	InternalStorage::~InternalStorage()
	{
	}

	int InternalStorage::Initialize(ID3D11Device4* pDevice4)
	{
		int flag = TRUE;
		char* pDataStart = m_cpuData;

		for (UINT i = 0; i < ALLOCATED_BUFFER_COUNT && flag; i++)
			flag = m_heaps[i].Initialize(pDevice4, BUFFER_TYPE(i), &pDataStart);

		return flag;
	}

	BufferHeap* InternalStorage::GetBufferHeapCPU(const BUFFER_TYPE type)
	{
		return &m_heaps[type];
	}

	ID3D11Buffer* InternalStorage::GetBufferHeapGPU(const BUFFER_TYPE type)
	{
		return m_heaps[type].m_pBuffer;
	}

	void InternalStorage::Release()
	{
		for (UINT i = 0; i < ALLOCATED_BUFFER_COUNT; i++)
		{
			m_heaps[i].Release();
		}
	}
}
