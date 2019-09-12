#include "Mesh.h"
// Running this constructor implies you want an empty mesh object
Mesh::Mesh()
{

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
	if (!this->mpIndexVector || !this->mpVertexPosVector || !this->mpNormalVector || !this->mpUVVector)
	{
		this->mpIndexVector = new std::vector<int>;
		this->mpVertexPosVector = new std::vector<DirectX::XMFLOAT3>;
		this->mpNormalVector = new std::vector<DirectX::XMFLOAT3>;
		this->mpUVVector = new std::vector<DirectX::XMFLOAT2>;
	}
	//FbxLoader::LoadFBX(filePath, this->mpVertexVector, this->mpIndexVector, &this->skeleton);
	return FbxLoader::LoadFBX(filePath, this->mpVertexPosVector, this->mpIndexVector, this->mpNormalVector, this->mpUVVector);

}

std::vector<DirectX::XMFLOAT3>* Mesh::GetVertexPositionVector()
{
	return this->mpVertexPosVector;
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
