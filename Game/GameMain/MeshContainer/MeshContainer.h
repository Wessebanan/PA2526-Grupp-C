#pragma once

#include <unordered_map>
#include "Mesh.h"
#include "MeshManager.h"
#include "../GameGlobals.h"

/*
	-- MeshContainer
	A singleton container for mesh data, making meshes accessible globaly.
	This singleton also contain information for the gpu mesh region per
	mesh.

	In order to retreive a mesh from MeshContainer, these has to be done:
		1. MeshContainer has been initialized with Initialize()
		2. The wanted mesh has been loaded with LoadMesh, and set to
		   the corresponding MESH_TYPE.

	The API for this class is all singleton. Before closing the application,
	MeshContainer::Terminate() has to be called in order to free allocated
	memory.
*/
class MeshContainer
{
public:

	// Has to be called before using MeshManager
	static void Initialize(graphics::MeshManager* pMeshMgr);

	// Loads a mesh from file into MeshContainer. The mesh can be retrieved after this.
	static ModelLoader::Mesh* LoadMesh(GAME_OBJECT_TYPE meshType, std::string filePath);

	// Returns a pointer to a requested mesh. The mesh has to be loaded before this is called.
	static ModelLoader::Mesh* GetMeshCPU(GAME_OBJECT_TYPE meshType);

	// Returns the GPU region for a requested mesh
	static graphics::MeshRegion GetMeshGPU(GAME_OBJECT_TYPE meshType);

	// Frees allocated memory in MeshContainer. Call this before exiting the application.
	static void Terminate();

private:

	// Hidden getters, used in static functions. If instance isn't created, its created here.
	static MeshContainer& GetInstance();

	MeshContainer();
	~MeshContainer();

	static MeshContainer* mpInstance;

	void InitializeInternal(graphics::MeshManager* pMeshMgr);
	ModelLoader::Mesh* LoadMeshInternal(GAME_OBJECT_TYPE meshType, std::string filePath);
	ModelLoader::Mesh* GetMeshCPUInternal(GAME_OBJECT_TYPE meshType);
	graphics::MeshRegion GetMeshGPUInternal(GAME_OBJECT_TYPE meshType);
	void TerminateInternal();

	graphics::MeshManager* mpMeshMgr;

	std::unordered_map<GAME_OBJECT_TYPE, ModelLoader::Mesh*> mMeshList;
	std::unordered_map<GAME_OBJECT_TYPE, graphics::MeshRegion> mMeshRegions;
};