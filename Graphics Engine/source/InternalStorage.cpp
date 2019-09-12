#include "../includes/InternalStorage.h"

namespace graphics
{

#define BUFFER_VERTEX_ARRAY_INDEX_LENGTH (1024)

	BufferHeap::BufferHeap()
	{
		m_currentSpent		= 0;
		m_size				= 0;
		m_pBuffer			= NULL;
		m_type				= BUFFER_CONSTANT_DYNAMIC;
		m_bindType			= D3D11_BIND_CONSTANT_BUFFER;
		m_currentCreated	= 0;

		ZeroMemory(m_cpuData, ARRAYSIZE(m_cpuData));
	}

	BufferHeap::~BufferHeap()
	{
	}

	void BufferHeap::Initialize(
		ID3D11Device4* pDevice4,
		BUFFER_TYPE type)
	{
		m_type = type;

		D3D11_BUFFER_DESC desc = { 0 };
		desc.CPUAccessFlags = 0;

		switch (type)
		{
		case BUFFER_CONSTANT_DYNAMIC:
			desc.BindFlags = m_bindType = D3D11_BIND_CONSTANT_BUFFER;
			desc.ByteWidth = ALLOCATED_BUFFER_SIZE;
			desc.Usage		= D3D11_USAGE_DYNAMIC;
			desc.MiscFlags = 0;
			break;

		case BUFFER_CONSTANT_STATIC:
			desc.BindFlags = m_bindType = D3D11_BIND_CONSTANT_BUFFER;
			desc.ByteWidth = ALLOCATED_BUFFER_SIZE;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.MiscFlags = 0;
			break;

		case BUFFER_RTV:
			desc.BindFlags = m_bindType = D3D11_BIND_RENDER_TARGET;
			desc.ByteWidth = ALLOCATED_BUFFER_SIZE;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.MiscFlags = 0;
			break;

		case BUFFER_SRV:
			desc.BindFlags = m_bindType = D3D11_BIND_SHADER_RESOURCE;
			desc.ByteWidth = ALLOCATED_BUFFER_SIZE;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.MiscFlags = 0;
			break;

		case BUFFER_INDEX:
			desc.BindFlags = m_bindType = D3D11_BIND_INDEX_BUFFER;
			desc.ByteWidth = ALLOCATED_BUFFER_SIZE;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.MiscFlags = 0;
			break;

		case BUFFER_VERTEX_POSITION:
			desc.BindFlags = m_bindType = D3D11_BIND_VERTEX_BUFFER;
			desc.ByteWidth = ALLOCATED_BUFFER_SIZE;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.MiscFlags = 0;
			break;

		case BUFFER_VERTEX_NORMAL:
			desc.BindFlags = m_bindType = D3D11_BIND_VERTEX_BUFFER;
			desc.ByteWidth = ALLOCATED_BUFFER_SIZE;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.MiscFlags = 0;
			break;

		case BUFFER_VERTEX_UV:
			desc.BindFlags = m_bindType = D3D11_BIND_VERTEX_BUFFER;
			desc.ByteWidth = ALLOCATED_BUFFER_SIZE;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.MiscFlags = 0;
			break;

		case BUFFER_VERTEX_ARRAY_INDEX:
			desc.BindFlags = m_bindType = D3D11_BIND_VERTEX_BUFFER;
			desc.ByteWidth = BUFFER_VERTEX_ARRAY_INDEX_LENGTH * sizeof(UINT);
			desc.Usage = D3D11_USAGE_IMMUTABLE;
			desc.MiscFlags = 0;
			break;

		default:
			break;
		}

		if (desc.Usage == D3D11_USAGE_DYNAMIC)
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;


		constexpr UINT elements = BUFFER_VERTEX_ARRAY_INDEX_LENGTH;
		UINT indices[elements];
		D3D11_SUBRESOURCE_DATA data = { 0 };

		switch (type)
		{
		case BUFFER_VERTEX_ARRAY_INDEX:
			for (UINT i = 0; i < elements; i++)
			{
				indices[i] = i;
			}

			data.pSysMem = indices;

			pDevice4->CreateBuffer(&desc, &data, &m_pBuffer);
			break;

		default:
			pDevice4->CreateBuffer(&desc, NULL, &m_pBuffer);
			break;
		}

		m_size = ALLOCATED_BUFFER_SIZE;
	}

	bool BufferHeap::AllocateRegion(const UINT size, BufferRegion* pRegion)
	{
		UINT allocatedSize = size;
		if (m_bindType == D3D11_BIND_CONSTANT_BUFFER)
		{
			allocatedSize = (UINT)ceil(allocatedSize / 256.0f);
			allocatedSize *= 256;
		}

		if (allocatedSize + m_currentSpent > m_size) return false;

		pRegion->DataLocation	= m_currentSpent;
		pRegion->Type			= m_type;
		pRegion->DataCount		= allocatedSize;
		pRegion->ID				= m_currentCreated;

		m_currentSpent		+= allocatedSize;
		m_currentCreated	+= 1;

		return true;
	}

	void BufferHeap::CopyDataToRegion(const void* pData, const BufferRegion& region)
	{
		switch (region.Type)
		{
		case BUFFER_VERTEX_ARRAY_INDEX:
			// DON'T UPLOAD ANYTHING
			// HAS ALL THE DATA IT NEEDS
			break;

		default:
			memcpy(
				m_cpuData + region.DataLocation,
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
				m_cpuData,
				m_currentSpent);

			pContext4->Unmap(m_pBuffer, 0);
			break;

		case BUFFER_CONSTANT_STATIC:
			pContext4->UpdateSubresource(
				m_pBuffer,
				0,
				NULL,
				m_cpuData,
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
				m_cpuData,
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
	}

	InternalStorage::~InternalStorage()
	{
	}

	void InternalStorage::Initialize(ID3D11Device4* pDevice4)
	{
		for (UINT i = 0; i < BUFFER_TYPE_LENGTH; i++)
			m_heaps[i].Initialize(pDevice4, BUFFER_TYPE(i));
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
		for (UINT i = 0; i < ALLOCATED_BUFFERS; i++)
		{
			m_heaps[i].Release();
		}
	}
}
