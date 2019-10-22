#include "MeshContainer.h"

MeshContainer* MeshContainer::mpInstance = nullptr;

MeshContainer& MeshContainer::GetInstance() // static
{
	if (!mpInstance)
	{
		mpInstance = new MeshContainer;
	}
	return *mpInstance;
}

ModelLoader::Mesh* MeshContainer::LoadMesh(MeshType meshType, std::string filePath) // static
{
	return GetInstance().LoadMeshInternal(meshType, filePath);
}

ModelLoader::Mesh* MeshContainer::GetMesh(MeshType meshType) // static
{
	return GetInstance().GetMeshInternal(meshType);
}

void MeshContainer::Terminate() // static
{
	if (mpInstance)
	{
		mpInstance->TerminateInternal();
	}
}

MeshContainer::MeshContainer()
{
	mMeshList.reserve((sizeof(ModelLoader::Mesh*) + sizeof(MeshType)) * MESH_TYPE_COUNT);
}

MeshContainer::~MeshContainer()
{
	TerminateInternal();
}

ModelLoader::Mesh* MeshContainer::LoadMeshInternal(MeshType meshType, std::string filePath)
{
	// Check if mesh is already loaded
	if (mMeshList.count(meshType))
	{
		return nullptr;
	}

	// Create mesh from file
	ModelLoader::Mesh* newMesh = new ModelLoader::Mesh(filePath);

	// Sanity check creation
	if (!newMesh)
	{
		return nullptr;
	}

	// Push new mesh into list (map)
	mMeshList[meshType] = newMesh;

	return newMesh;
}

ModelLoader::Mesh* MeshContainer::GetMeshInternal(MeshType meshType)
{
	// Check if mesh is exist
	if (!mMeshList.count(meshType))
	{
		return nullptr;
	}

	return mMeshList[meshType];
}

void MeshContainer::TerminateInternal()
{
	for (std::pair<MeshType, ModelLoader::Mesh*> mesh_pair : mMeshList)
	{
		delete mesh_pair.second;
	}
	mMeshList.clear();
}
