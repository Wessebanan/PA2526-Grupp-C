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
		bool mHasSkeleton = false;

	public:
		Mesh();
		Mesh(const std::string& filePath);
		~Mesh();
		// Populates the objects vectors with data from provided .fbx filepath
		HRESULT LoadFBX(const std::string& filePath);

		// Get pointer to XMFLOAT3 vector of vertex positions
		std::vector<DirectX::XMFLOAT3>* GetVertexPositionVector();
		// Get pointer to int vector of indices for index buffer
		std::vector<int>* GetIndexVector();
		// Get pointer to XMFLOAT3 vector of vertex normals
		std::vector<DirectX::XMFLOAT3>* GetNormalVector();
		// Get pointer to XMFLOAT2 vector of vertex UV coords
		std::vector<DirectX::XMFLOAT2>* GetUVVector();
		// Get Skeleton structure of joints
		ModelLoader::Skeleton* GetSkeleton();
		// Get vector of ControlPointInfo, containing arrays of 4 IndexWeightPairs, for use in skinning shader
		std::vector<ModelLoader::ControlPointInfo>* GetSkinningWeights();

		bool HasUVs();
		bool HasNormals();
		bool HasSkeleton();
	};
}
