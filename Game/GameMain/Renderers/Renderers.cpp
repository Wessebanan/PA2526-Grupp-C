#include "Renderers.h"
#include "../gameAI/AIComponents.h"
#include "../gameUtility/UtilityComponents.h"
#include "../gameUtility/UtilityEcsFunctions.h"
#include "../gameUtility/UtilityGraphics.h"
#include "../gameSceneObjects/SceneObjectComponents.h"
#include "../gameWorld/OceanComponents.h"

namespace ecs
{
	namespace systems
	{
#pragma region UnitRenderSystem
		UnitRenderSystem::UnitRenderSystem() : mFrameCounter(0), mAnimationFrameCounter(0), mUnitCount(0), mpRenderBuffer(0)
		{
			updateType = SystemUpdateType::MultiEntityUpdate;
			typeFilter.addRequirement(components::UnitComponent::typeID);
			typeFilter.addRequirement(components::TransformComponent::typeID);
			typeFilter.addRequirement(components::ColorComponent::typeID);

			mInstanceLayout = { 0 };

			mpSkeleton = MeshContainer::GetMeshCPU(MESH_TYPE_UNIT)->GetSkeleton();
		}

		UnitRenderSystem::~UnitRenderSystem()
		{

		}

		void UnitRenderSystem::updateMultipleEntities(EntityIterator& _entities, float _delta)
		{
			// Update object count to render
			mUnitCount = (UINT)_entities.entities.size();

			// Fetch pointer to write data to in RenderBuffer
			mpBuffer = (InputLayout*)mpRenderBuffer->GetBufferAddress(mUnitCount * GetPerInstanceSize());

			// Iterate all units and write their data to the RenderBuffer
			int index = 0;
			for (FilteredEntity unit : _entities.entities)
			{
				components::TransformComponent* p_transform_comp = unit.getComponent<ecs::components::TransformComponent>();
				components::ColorComponent* p_color_comp = unit.getComponent<ecs::components::ColorComponent>();
				DirectX::XMMATRIX world = UtilityEcsFunctions::GetWorldMatrix(*p_transform_comp);

				XMStoreFloat4x4(&mpBuffer[index].world, world);

				mpBuffer[index].world._44 = PACK(
					p_color_comp->red,
					p_color_comp->green,
					p_color_comp->blue,
					0);

				memcpy(
					mpBuffer[index].boneMatrices,
					&mpSkeleton->animationData[mAnimationFrameCounter * mpSkeleton->jointCount],
					mpSkeleton->jointCount * sizeof(DirectX::XMFLOAT4X4));

				index++;
			}

			// Update animation every 5th frame
			mFrameCounter++;
			if (mFrameCounter % 5 == 0)
			{
				mFrameCounter = 0;
				mAnimationFrameCounter = ++mAnimationFrameCounter % mpSkeleton->frameCount;
			}

			mpRenderMgr->SetShaderModelLayout(mRenderProgram, mInstanceLayout);
		}

		void UnitRenderSystem::Initialize(graphics::RenderManager* pRenderMgr, graphics::RenderBuffer* pRenderBuffer)
		{
			mpRenderMgr = pRenderMgr;
			mUnitMeshRegion = MeshContainer::GetMeshGPU(MESH_TYPE_UNIT);

			mInstanceLayout.MeshCount = 1;
			mInstanceLayout.pMeshes = &mUnitMeshRegion;
			mInstanceLayout.pInstanceCountPerMesh = &mUnitCount;

			const std::string ps = GetShaderFilepath("PS_Default.cso");
			const std::string vs_skin = GetShaderFilepath("VS_Skinning.cso");

			mRenderProgram = mpRenderMgr->CreateShaderProgram(
				vs_skin.c_str(),
				ps.c_str(),
				systems::UnitRenderSystem::GetPerInstanceSize());

			mpRenderBuffer = pRenderBuffer;
		}

		uint32_t UnitRenderSystem::GetPerInstanceSize()
		{
			return sizeof(InputLayout);
		}
#pragma endregion UnitRenderSystem

#pragma region TileRenderSystem
		TileRenderSystem::TileRenderSystem()
		{
			updateType = SystemUpdateType::MultiEntityUpdate;
			typeFilter.addRequirement(components::TileComponent::typeID);
			typeFilter.addRequirement(components::TransformComponent::typeID);
			typeFilter.addRequirement(components::ColorComponent::typeID);

			mInstanceLayout = { 0 };
		}

		TileRenderSystem::~TileRenderSystem()
		{

		}

		void TileRenderSystem::updateMultipleEntities(EntityIterator& _entities, float _delta)
		{
			mTileCount = (UINT)_entities.entities.size();

			// Fetch pointer to write data to in RenderBuffer
			mpBuffer = (InputLayout*)mpRenderBuffer->GetBufferAddress(mTileCount * GetPerInstanceSize());

			// Iterate all tiles and write their data to the RenderBuffer
			uint32_t index = 0;
			for (FilteredEntity tile: _entities.entities)
			{
				components::TileComponent* p_tile_comp = tile.getComponent<components::TileComponent>();
				components::TransformComponent* p_transform_comp = tile.getComponent<components::TransformComponent>();
				components::ColorComponent* p_color_comp = tile.getComponent<components::ColorComponent>();

				mpBuffer[index].x = p_transform_comp->position.x;
				mpBuffer[index].y = p_transform_comp->position.y;
				mpBuffer[index].z = p_transform_comp->position.z;

				uint32_t random = (uint32_t)rand() % 101;
				uint32_t color_offset = -50 + random;
				switch (p_tile_comp->tileType)
				{
				case TileTypes::GAME_FIELD:
					mpBuffer[index].color = PACK(p_color_comp->red, p_color_comp->green, p_color_comp->blue, 0);
					break;
				case TileTypes::WATER:
					mpBuffer[index].color = PACK(0, 0, 200 + color_offset, 0);
					break;
				case TileTypes::UNDEFINED:
					mpBuffer[index].color = PACK(0, 0, 0, 255);
					break;
				default:
					mpBuffer[index].color = PACK(255, 255, 255, 255);
					break;
				}

				index++;
			}

			mpRenderMgr->SetShaderModelLayout(mRenderProgram, mInstanceLayout);
		}

		void TileRenderSystem::Initialize(graphics::RenderManager* pRenderMgr, graphics::RenderBuffer* pRenderBuffer)
		{
			mpRenderMgr = pRenderMgr;
			mTileMeshRegion = MeshContainer::GetMeshGPU(MESH_TYPE_TILE);

			mInstanceLayout.MeshCount = 1;
			mInstanceLayout.pMeshes = &mTileMeshRegion;
			mInstanceLayout.pInstanceCountPerMesh = &mTileCount;

			const std::string vs = GetShaderFilepath("VS_Default.cso");
			const std::string ps = GetShaderFilepath("PS_Default.cso");

			mRenderProgram = mpRenderMgr->CreateShaderProgram(
				vs.c_str(),
				ps.c_str(),
				systems::TileRenderSystem::GetPerInstanceSize());

			mpRenderBuffer = pRenderBuffer;
		}

		uint32_t TileRenderSystem::GetPerInstanceSize()
		{
			return sizeof(InputLayout);
		}
#pragma endregion TileRenderSystem

#pragma region OceanRenderSystem
	OceanRenderSystem::OceanRenderSystem()
	{
		updateType = SystemUpdateType::MultiEntityUpdate;
		typeFilter.addRequirement(components::OceanTileComponent::typeID);
		typeFilter.addRequirement(components::TransformComponent::typeID);
		typeFilter.addRequirement(components::ColorComponent::typeID);

		mInstanceLayout = { 0 };
	}

	OceanRenderSystem::~OceanRenderSystem()
	{

	}

	void OceanRenderSystem::updateMultipleEntities(EntityIterator& _entities, float _delta)
	{
		mTileCount = (UINT)_entities.entities.size();

		// Fetch pointer to write data to in RenderBuffer
		mpBuffer = (InputLayout*)mpRenderBuffer->GetBufferAddress(mTileCount * OceanRenderSystem::GetPerInstanceSize());

		// Iterate all tiles and write their data to the RenderBuffer
		uint32_t index = 0;
		for (FilteredEntity tile : _entities.entities)
		{
			components::TileComponent* p_tile_comp = tile.getComponent<components::TileComponent>();
			components::TransformComponent* p_transform_comp = tile.getComponent<components::TransformComponent>();
			components::ColorComponent* p_color_comp = tile.getComponent<components::ColorComponent>();

			mpBuffer[index].x = p_transform_comp->position.x;
			mpBuffer[index].y = p_transform_comp->position.y;
			mpBuffer[index].z = p_transform_comp->position.z;

			mpBuffer[index].color = PACK(p_color_comp->red, p_color_comp->green, p_color_comp->blue, 0);

			index++;
		}

		mpRenderMgr->SetShaderModelLayout(mRenderProgram, mInstanceLayout);
	}

	void OceanRenderSystem::Initialize(graphics::RenderManager* pRenderMgr, graphics::RenderBuffer* pRenderBuffer)
	{
		mpRenderMgr = pRenderMgr;
		mTileMeshRegion = MeshContainer::GetMeshGPU(MESH_TYPE_TILE);

		mInstanceLayout.MeshCount = 1;
		mInstanceLayout.pMeshes = &mTileMeshRegion;
		mInstanceLayout.pInstanceCountPerMesh = &mTileCount;

		const std::string vs = GetShaderFilepath("VS_Default.cso");
		const std::string ps = GetShaderFilepath("PS_Default.cso");

		mRenderProgram = mpRenderMgr->CreateShaderProgram(
			vs.c_str(),
			ps.c_str(),
			systems::TileRenderSystem::GetPerInstanceSize());

		mpRenderBuffer = pRenderBuffer;
	}

	uint32_t OceanRenderSystem::GetPerInstanceSize()
	{
		return sizeof(InputLayout);
	}
#pragma endregion OceanRenderSystem

#pragma region SceneObjectRenderSystem
		SceneObjectRenderSystem::SceneObjectRenderSystem()
		{
			updateType = SystemUpdateType::MultiEntityUpdate;
			typeFilter.addRequirement(components::SceneObjectComponent::typeID);
			typeFilter.addRequirement(components::TransformComponent::typeID);
			typeFilter.addRequirement(components::ColorComponent::typeID);

			mInstanceLayout = { 0 };
		}

		SceneObjectRenderSystem::~SceneObjectRenderSystem()
		{

		}

		void SceneObjectRenderSystem::updateMultipleEntities(EntityIterator& _entities, float _delta)
		{
			/*
				We don't know the order of entities EntityIterator, meaning that we can't expect
				the entities to be ordered by mesh type like we want them to be in the RenderBuffer
				(output of this function).

				So, this function first calculate how many instances we have of each mesh. With this,
				we can calculate from which index in the RenderBuffer we can start writing each mesh
				to. Each mesh we care about in this function, that is tree, stone etc., has its own
				index counter.
			*/

			mObjectCount = _entities.entities.size();

			// Fetch pointer to write data to in RenderBuffer
			mpBuffer = (InputLayout*)mpRenderBuffer->GetBufferAddress(mObjectCount * systems::SceneObjectRenderSystem::GetPerInstanceSize());

			// Count how many instances we have per scene object mesh
			ZeroMemory(mObjectTypeCount, SCENE_OBJECT_COUNT * sizeof(UINT));
			for (FilteredEntity object : _entities.entities)
			{
				components::SceneObjectComponent* p_obj_comp = object.getComponent<components::SceneObjectComponent>();
				mObjectTypeCount[p_obj_comp->mObject]++;
			}

			// Set index to write to in RenderBuffer, per mesh
			UINT object_type_individual_index[SCENE_OBJECT_COUNT] = { 0 };

			for (int i = 1; i < SCENE_OBJECT_COUNT; i++)
			{
				object_type_individual_index[i] = object_type_individual_index[i - 1] + mObjectTypeCount[i - 1];
			}

			// Iterate all objects and write their data to the RenderBuffer
			for (FilteredEntity object : _entities.entities)
			{
				components::SceneObjectComponent* p_obj_comp = object.getComponent<components::SceneObjectComponent>();
				components::TransformComponent* p_transform_comp = object.getComponent<components::TransformComponent>();
				components::ColorComponent* p_color_comp = object.getComponent<components::ColorComponent>();

				// Get index, depending on mesh type
				UINT& index = object_type_individual_index[p_obj_comp->mObject];

				mpBuffer[index].x = p_transform_comp->position.x;
				mpBuffer[index].y = p_transform_comp->position.y;
				mpBuffer[index].z = p_transform_comp->position.z;

				mpBuffer[index].color = PACK(p_color_comp->red, p_color_comp->green, p_color_comp->blue, 0);
				index++;
			}

			mInstanceLayout.pInstanceCountPerMesh = mObjectTypeCount;
			mpRenderMgr->SetShaderModelLayout(mRenderProgram, mInstanceLayout);
		}

		void SceneObjectRenderSystem::Initialize(graphics::RenderManager* pRenderMgr, graphics::RenderBuffer* pRenderBuffer)
		{
			mpRenderMgr = pRenderMgr;

			/*
				This is a temporary map, until we have ONE mesh enum that everyone reads from.
				This converts the SCENE_OBJECT mesh enum to MESH_TYPE enum in MeshContainer.
			*/
			mMeshMap[SCENE_OBJECT::SNOWMAN]		= MESH_TYPE::MESH_TYPE_TREE;
			mMeshMap[SCENE_OBJECT::ANGEL]		= MESH_TYPE::MESH_TYPE_TREE;
			mMeshMap[SCENE_OBJECT::IGLOO]		= MESH_TYPE::MESH_TYPE_TREE;
			mMeshMap[SCENE_OBJECT::CLIFF]		= MESH_TYPE::MESH_TYPE_TREE;
			mMeshMap[SCENE_OBJECT::ROCKS]		= MESH_TYPE::MESH_TYPE_ROCK;
			mMeshMap[SCENE_OBJECT::CAMP]		= MESH_TYPE::MESH_TYPE_TREE;
			mMeshMap[SCENE_OBJECT::TREES]		= MESH_TYPE::MESH_TYPE_TREE;
			mMeshMap[SCENE_OBJECT::FLOWERS]		= MESH_TYPE::MESH_TYPE_TREE;
			mMeshMap[SCENE_OBJECT::VILAGE]		= MESH_TYPE::MESH_TYPE_TREE;
			mMeshMap[SCENE_OBJECT::SANDSTONE]	= MESH_TYPE::MESH_TYPE_TREE;
			mMeshMap[SCENE_OBJECT::PALMS]		= MESH_TYPE::MESH_TYPE_TREE;
			mMeshMap[SCENE_OBJECT::PYRAMIDS]	= MESH_TYPE::MESH_TYPE_TREE;


			for (int i = 0; i < SCENE_OBJECT_COUNT; i++)
			{
				mObjectMeshRegion[i] = MeshContainer::GetMeshGPU(mMeshMap[i]);
			}

			mInstanceLayout.MeshCount = SCENE_OBJECT_COUNT;
			mInstanceLayout.pMeshes = mObjectMeshRegion;
			mInstanceLayout.pInstanceCountPerMesh = &mObjectCount;

			const std::string vs = GetShaderFilepath("VS_Default.cso");
			const std::string ps = GetShaderFilepath("PS_Default.cso");

			mRenderProgram = mpRenderMgr->CreateShaderProgram(
				vs.c_str(),
				ps.c_str(),
				systems::SceneObjectRenderSystem::GetPerInstanceSize());

			mpRenderBuffer = pRenderBuffer;
		}

		uint32_t SceneObjectRenderSystem::GetPerInstanceSize()
		{
			return sizeof(InputLayout);
		}
#pragma endregion SceneObjectRenderSystem
	}
}