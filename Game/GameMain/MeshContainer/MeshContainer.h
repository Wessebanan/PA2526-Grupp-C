#pragma once

#include <unordered_map>
#include "Mesh.h"

typedef unsigned int MeshType;
enum MESH_TYPE
{
	MESH_TYPE_TILE,
	MESH_TYPE_UNIT,
	MESH_TYPE_ROCK,
	MESH_TYPE_TREE,

	MESH_TYPE_COUNT
};

class MeshContainer
{
public:

	static MeshContainer& GetInstance();
	static ModelLoader::Mesh* LoadMesh(MeshType meshType, std::string filePath);
	static ModelLoader::Mesh* GetMesh(MeshType meshType);
	static void Terminate();

private:
	MeshContainer();
	~MeshContainer();

	static MeshContainer* mpInstance;

	ModelLoader::Mesh* LoadMeshInternal(MeshType meshType, std::string filePath);
	ModelLoader::Mesh* GetMeshInternal(MeshType meshType);
	void TerminateInternal();

	std::unordered_map<MeshType, ModelLoader::Mesh*> mMeshList;
};