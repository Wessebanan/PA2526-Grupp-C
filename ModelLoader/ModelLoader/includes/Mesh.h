#pragma once
#include "Fbx_loader.h"

class Mesh {
private:
	std::vector<DirectX::XMFLOAT3>* mpVertexPosVector = nullptr;
	std::vector<int>* mpIndexVector = nullptr;
	std::vector<DirectX::XMFLOAT3>* mpNormalVector = nullptr;
	std::vector<DirectX::XMFLOAT2>* mpUVVector = nullptr;


public:
	Mesh();
	~Mesh();
	// Populates the objects vectors with data from provided .fbx filepath
	HRESULT LoadFBX(const std::string& filePath);

	std::vector<DirectX::XMFLOAT3>* GetVertexPositionVector();
	std::vector<int>* GetIndexVector();
	std::vector<DirectX::XMFLOAT3>* GetNormalVector();
	std::vector<DirectX::XMFLOAT2>* GetUVVector();
};