#pragma once

#include <unordered_map>
#include "Mesh.h"
#include "../../Graphics/includes/MeshManager.h"

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

	static void Initialize(graphics::MeshManager* pMeshMgr);

	static MeshContainer& GetInstance();
	static ModelLoader::Mesh* LoadMesh(MeshType meshType, std::string filePath);
	static ModelLoader::Mesh* GetMeshCPU(MeshType meshType);
	static graphics::MeshRegion GetMeshGPU(MeshType meshType);
	static void Terminate();

private:
	MeshContainer();
	~MeshContainer();

	static MeshContainer* mpInstance;

	void InitializeInternal(graphics::MeshManager* pMeshMgr);
	ModelLoader::Mesh* LoadMeshInternal(MeshType meshType, std::string filePath);
	ModelLoader::Mesh* GetMeshCPUInternal(MeshType meshType);
	graphics::MeshRegion GetMeshGPUInternal(MeshType meshType);
	void TerminateInternal();

	graphics::MeshManager* mpMeshMgr;

	std::unordered_map<MeshType, ModelLoader::Mesh*> mMeshList;
	std::unordered_map<MeshType, graphics::MeshRegion> mMeshRegions;
};