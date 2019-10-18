#include "Mesh.h"
// Running this constructor implies you want an empty mesh object
ModelLoader::Mesh::Mesh()
{

}

ModelLoader::Mesh::Mesh(const std::string& filePath)
{
	this->LoadFBX(filePath);
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
	if (!this->mpIndexVector || !this->mpVertexPosVector || !this->mpNormalVector || !this->mpUVVector)
	{
		this->mpIndexVector = new std::vector<int>;
		this->mpVertexPosVector = new std::vector<DirectX::XMFLOAT3>;
		this->mpNormalVector = new std::vector<DirectX::XMFLOAT3>;
		this->mpUVVector = new std::vector<DirectX::XMFLOAT2>;
		this->mpSkeleton = new ModelLoader::Skeleton();
		this->mpSkinningWeights = new std::vector<ModelLoader::ControlPointInfo>;
	}
	HRESULT hr = E_FAIL;
	try
	{
		hr = ModelLoader::LoadFBX(filePath, this->mpVertexPosVector, this->mpIndexVector, this->mpNormalVector, this->mpUVVector, this->mpSkeleton, this->mpSkinningWeights);
	}
	catch (std::exception e)
	{
		MessageBoxA(NULL, e.what(), "Error in FBX Loader.", MB_OK);
	}

	if (SUCCEEDED(hr))
	{
		// Assume UVs and normals exist
		this->mHasUvs = true;
		this->mHasNormals = true;
		this->mHasSkeleton = true;
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
		if (this->mpSkeleton->joints.size() <= 0)
		{
			delete this->mpSkeleton;
			delete this->mpSkinningWeights;
			this->mpSkinningWeights = nullptr;
			mpSkeleton = nullptr;
			this->mHasSkeleton = false;
		}
	}
	else
	{
		delete mpVertexPosVector;
		delete mpIndexVector;
		delete mpNormalVector;
		delete mpUVVector;
		delete mpSkinningWeights;
		delete mpSkeleton;
		mpVertexPosVector = nullptr;
		mpUVVector = nullptr;
		mpNormalVector = nullptr;
		mpIndexVector = nullptr;
		mpSkinningWeights = nullptr;
		mpSkeleton = nullptr;
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

ModelLoader::Skeleton* ModelLoader::Mesh::GetSkeleton()
{
	return this->mpSkeleton;
}

std::vector<ModelLoader::ControlPointInfo>* ModelLoader::Mesh::GetSkinningWeights()
{
	return this->mpSkinningWeights;
}

bool ModelLoader::Mesh::HasUVs()
{
	return mHasUvs;
}

bool ModelLoader::Mesh::HasNormals()
{
	return this->mHasNormals;
}

bool ModelLoader::Mesh::HasSkeleton()
{
	return this->mHasSkeleton;
}
