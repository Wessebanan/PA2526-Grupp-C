#include "Mesh.h"
// Running this constructor implies you want an empty mesh object
ModelLoader::Mesh::Mesh()
{
	mpVertexPosVector = nullptr;
	mpUVVector = nullptr;
	mpNormalVector = nullptr;
	mpIndexVector = nullptr;
}



ModelLoader::Mesh::~Mesh()
{
	delete mpVertexPosVector;
	delete mpIndexVector;
	delete mpNormalVector;
	delete mpUVVector;
}

HRESULT ModelLoader::Mesh::LoadFBX(const std::string& filePath)
{
	// Create vectors if they dont already exist
	if (!this->mpIndexVector || !this->mpVertexPosVector || !this->mpNormalVector || !this->mpUVVector)
	{
		this->mpIndexVector = new std::vector<int>;
		this->mpVertexPosVector = new std::vector<DirectX::XMFLOAT3>;
		this->mpNormalVector = new std::vector<DirectX::XMFLOAT3>;
		this->mpUVVector = new std::vector<DirectX::XMFLOAT2>;
	}
	HRESULT hr = ModelLoader::LoadFBX(filePath, this->mpVertexPosVector, this->mpIndexVector, this->mpNormalVector, this->mpUVVector);
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
	else
	{
		// Assume UVs and normals exist
		this->mHasUvs = true;
		this->mHasNormals = true;
		if (this->mpNormalVector->size() <= 0)
		{
			delete mpNormalVector;
			mpNormalVector = nullptr;
			this->mHasNormals = false;
		}
		if (this->mpUVVector->size() <= 0)
		{
			delete mpUVVector;
			mpUVVector = nullptr;
			this->mHasUvs = false;
		}
	}
	return hr;

}

std::vector<DirectX::XMFLOAT3>* ModelLoader::Mesh::GetVertexPositionVector()
{
	if (this->mpVertexPosVector)
		return this->mpVertexPosVector;
	else
		return nullptr;
}

std::vector<int>* ModelLoader::Mesh::GetIndexVector()
{
	return this->mpIndexVector;
}

std::vector<DirectX::XMFLOAT3>* ModelLoader::Mesh::GetNormalVector()
{
	return this->mpNormalVector;
}

std::vector<DirectX::XMFLOAT2>* ModelLoader::Mesh::GetUVVector()
{
	return this->mpUVVector;
}

bool ModelLoader::Mesh::HasUVs()
{
	return mHasUvs;
}

bool ModelLoader::Mesh::HasNormals()
{
	return this->mHasNormals;
}
