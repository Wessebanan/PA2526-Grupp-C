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
	delete mpSkinningWeights;
	delete mpSkeleton;
	delete this->mpBlendIndices;
	delete this->mpBlendWeights;
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
		this->mpBlendIndices = new std::vector<unsigned int>;
		this->mpBlendWeights = new std::vector<double>;
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
			delete this->mpBlendIndices;
			delete this->mpBlendWeights;
			this->mpSkinningWeights = nullptr;
			this->mpBlendIndices = nullptr;
			this->mpBlendWeights = nullptr;
			mpSkeleton = nullptr;
			this->mHasSkeleton = false;
		}
		else
		{
			this->mpBlendIndices->reserve(this->mpVertexPosVector->size() * 4);
			this->mpBlendWeights->reserve(this->mpVertexPosVector->size() * 3);
			for (auto a : *this->mpSkinningWeights)
			{
				for (unsigned int i = 0; i < 3; i++)
				{
					this->mpBlendIndices->push_back(a.weightPairs[i].index);
					this->mpBlendWeights->push_back(a.weightPairs[i].weight);
				}
				this->mpBlendIndices->push_back(a.weightPairs[3].index);
			}		
			delete this->mpSkinningWeights;
			this->mpSkinningWeights = nullptr;
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
		delete this->mpBlendIndices;
		delete this->mpBlendWeights;
		mpVertexPosVector = nullptr;
		mpUVVector = nullptr;
		mpNormalVector = nullptr;
		mpIndexVector = nullptr;
		this->mpSkinningWeights = nullptr;
		this->mpBlendIndices = nullptr;
		this->mpBlendWeights = nullptr;
		
		mpSkeleton = nullptr;
	}

	return hr;

}

std::vector<DirectX::XMFLOAT3>* ModelLoader::Mesh::GetVertexPositionVector()
{
	return this->mpVertexPosVector;
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

std::vector<unsigned int>* ModelLoader::Mesh::GetBlendIndices()
{
	return this->mpBlendIndices;
}

std::vector<double>* ModelLoader::Mesh::GetBlendWeights()
{
	return this->mpBlendWeights;
}


bool ModelLoader::Mesh::HasUVs()
{
	return this->mHasUvs;
}

bool ModelLoader::Mesh::HasNormals()
{
	return this->mHasNormals;
}

bool ModelLoader::Mesh::HasSkeleton()
{
	return this->mHasSkeleton;
}

