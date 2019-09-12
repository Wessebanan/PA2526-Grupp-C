#include "Mesh.h"
// Running this constructor implies you want an empty mesh object
Mesh::Mesh()
{
	mpVertexPosVector = nullptr;
	mpUVVector = nullptr;
	mpNormalVector = nullptr;
	mpIndexVector = nullptr;
}

Mesh::~Mesh()
{
	delete mpVertexPosVector;
	delete mpIndexVector;
	delete mpNormalVector;
	delete mpUVVector;
}

HRESULT Mesh::LoadFBX(const std::string& filePath)
{
	// Create vectors if they dont already exist
	if (!this->mpIndexVector || !this->mpVertexPosVector || !this->mpNormalVector || !this->mpUVVector)
	{
		this->mpIndexVector = new std::vector<int>;
		this->mpVertexPosVector = new std::vector<DirectX::XMFLOAT3>;
		this->mpNormalVector = new std::vector<DirectX::XMFLOAT3>;
		this->mpUVVector = new std::vector<DirectX::XMFLOAT2>;
	}
	HRESULT hr = FbxLoader::LoadFBX(filePath, this->mpVertexPosVector, this->mpIndexVector, this->mpNormalVector, this->mpUVVector);
	// Load failed, clean memory
	if (FAILED(hr))
	{
		delete mpVertexPosVector;
		delete mpIndexVector;
		delete mpNormalVector;
		delete mpUVVector;
		mpVertexPosVector = nullptr;
		mpUVVector = nullptr;
		mpNormalVector = nullptr;
		mpIndexVector = nullptr;
	}
	return hr;

}

std::vector<DirectX::XMFLOAT3>* Mesh::GetVertexPositionVector()
{
	if (this->mpVertexPosVector)
		return this->mpVertexPosVector;
	else
		return nullptr;
}

std::vector<int>* Mesh::GetIndexVector()
{
	return this->mpIndexVector;
}

std::vector<DirectX::XMFLOAT3>* Mesh::GetNormalVector()
{
	return this->mpNormalVector;
}

std::vector<DirectX::XMFLOAT2>* Mesh::GetUVVector()
{
	return this->mpUVVector;
}
