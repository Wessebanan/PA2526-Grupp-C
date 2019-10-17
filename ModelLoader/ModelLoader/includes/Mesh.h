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

		std::vector<DirectX::XMFLOAT3>* GetVertexPositionVector();
		std::vector<int>* GetIndexVector();
		std::vector<DirectX::XMFLOAT3>* GetNormalVector();
		std::vector<DirectX::XMFLOAT2>* GetUVVector();

		ModelLoader::Skeleton* GetSkeleton();
		std::vector<ModelLoader::ControlPointInfo>* GetSkinningWeights();

		bool HasUVs();
		bool HasNormals();
		bool HasSkeleton();
	};
}
