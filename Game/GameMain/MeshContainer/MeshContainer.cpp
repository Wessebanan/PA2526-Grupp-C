#include "MeshContainer.h"

MeshContainer* MeshContainer::mpInstance = nullptr;

void MeshContainer::Initialize(graphics::MeshManager* pMeshMgr) // static
{
	GetInstance().InitializeInternal(pMeshMgr);
}

MeshContainer& MeshContainer::GetInstance() // static
{
	if (!mpInstance)
	{
		mpInstance = new MeshContainer;
	}
	return *mpInstance;
}

ModelLoader::Mesh* MeshContainer::LoadMesh(GAME_OBJECT_TYPE meshType, std::string filePath) // static
{
	return GetInstance().LoadMeshInternal(meshType, filePath);
}

ModelLoader::Mesh* MeshContainer::GetMeshCPU(GAME_OBJECT_TYPE meshType) // static
{
	return GetInstance().GetMeshCPUInternal(meshType);
}

graphics::MeshRegion MeshContainer::GetMeshGPU(GAME_OBJECT_TYPE meshType)
{
	return GetInstance().GetMeshGPUInternal(meshType);
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
	mMeshList.reserve(MESH_TYPE_COUNT);
	mMeshRegions.reserve(MESH_TYPE_COUNT);
}

MeshContainer::~MeshContainer()
{
	TerminateInternal();
}

void MeshContainer::InitializeInternal(graphics::MeshManager* pMeshMgr)
{
	mpMeshMgr = pMeshMgr;
}

ModelLoader::Mesh* MeshContainer::LoadMeshInternal(GAME_OBJECT_TYPE meshType, std::string filePath)
{
	// Check if mesh is already loaded
	if (mMeshList.count(meshType))
	{
		return nullptr;
	}

	// Create mesh from file
	ModelLoader::Mesh* p_new_mesh = new ModelLoader::Mesh(filePath);

	// Sanity check creation
	if (!p_new_mesh)
	{
		return nullptr;
	}

	// Push new mesh into list (map)
	mMeshList[meshType] = p_new_mesh;

	// Create GPU region for mesh
	graphics::MeshRegion mesh_region;

	{
		mesh_region = mpMeshMgr->CreateMeshRegion(
			p_new_mesh->GetVertexPositionVector()->size(),
			p_new_mesh->GetIndexVector()->size());

		{
			graphics::VERTEX_DATA data = { NULL };
			data.pVertexPositions = p_new_mesh->GetVertexPositionVector()->data();
			data.pVertexNormals = p_new_mesh->GetNormalVector()->data();
			data.pVertexTexCoords = p_new_mesh->GetUVVector()->data();
			if (p_new_mesh->HasSkeleton())
			{
				data.pVertexBlendWeights = p_new_mesh->GetBlendWeights()->data();
				data.pVertexBlendIndices = p_new_mesh->GetBlendIndices()->data();
			}


			mpMeshMgr->UploadData(
				mesh_region,
				data,
				p_new_mesh->GetIndexVector()->data());
		}
	}
	mMeshRegions[meshType] = mesh_region;

	return p_new_mesh;
}

ModelLoader::Mesh* MeshContainer::GetMeshCPUInternal(GAME_OBJECT_TYPE meshType)
{
	// Check if mesh exist, else return null
	if (!mMeshList.count(meshType))
	{
		return nullptr;
	}

	return mMeshList[meshType];
}

graphics::MeshRegion MeshContainer::GetMeshGPUInternal(GAME_OBJECT_TYPE meshType)
{
	// Check if region exist, else return null
	if (!mMeshRegions.count(meshType))
	{
		return { 0, 0 };
	}

	return mMeshRegions[meshType];
}

void MeshContainer::TerminateInternal()
{
	// Free all allocated memory
	for (std::pair<GAME_OBJECT_TYPE, ModelLoader::Mesh*> mesh_pair : mMeshList)
	{
		delete mesh_pair.second;
	}
	mMeshList.clear();
	mMeshRegions.clear();
}
