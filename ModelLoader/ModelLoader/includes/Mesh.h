#pragma once
#include "Fbx_loader.h"

namespace ModelLoader {
	class Mesh {
	private:
		std::vector<DirectX::XMFLOAT3>* mpVertexPosVector = nullptr;
		std::vector<int>* mpIndexVector = nullptr;
		std::vector<DirectX::XMFLOAT3>* mpNormalVector = nullptr;
		std::vector<DirectX::XMFLOAT2>* mpUVVector = nullptr;

		bool mHasUvs = false;
		bool mHasNormals = false;

		// Used for skinning / skeletal animation
		ModelLoader::Skeleton* mpSkeleton = nullptr;
		std::vector<ModelLoader::ControlPointInfo>* mpSkinningWeights = nullptr;
		std::vector<unsigned int>* mpBlendIndices;
		std::vector<float>* mpBlendWeights;
		bool mHasSkeleton = false;

	public:
		Mesh();
		Mesh(const std::string& filePath);
		~Mesh();
		// Populates the vectors with data from provided .fbx filepath
		HRESULT LoadFBX(const std::string& filePath);

		std::vector<DirectX::XMFLOAT3>* GetVertexPositionVector();
		std::vector<int>* GetIndexVector();
		std::vector<DirectX::XMFLOAT3>* GetNormalVector();
		std::vector<DirectX::XMFLOAT2>* GetUVVector();

		ModelLoader::Skeleton* GetSkeleton();
		// Retrieve the vector containing all blend indices in the mesh
		// Size: 4 indices per vertex in the mesh
		std::vector<unsigned int>* GetBlendIndices();
		// Retrieve the vector containing all blend weights in the mesh
		// Size: 3 weights per vertex in the mesh
		std::vector<float>* GetBlendWeights();

		bool HasUVs();
		bool HasNormals();
		bool HasSkeleton();
	};
}
