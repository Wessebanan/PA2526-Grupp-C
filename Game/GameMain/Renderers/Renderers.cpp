#include "Renderers.h"
#include "../gameAI/AIComponents.h"
#include "../gameUtility/UtilityComponents.h"
#include "../gameUtility/UtilityEcsFunctions.h"
#include "../gameUtility/UtilityGraphics.h"
#include "../gameSceneObjects/SceneObjectComponents.h"
#include "../gameWorld/WorldComponents.h"
#include "../gameAnimation/AnimationComponents.h"

#include "../gameGraphics/TileRenderingPipeline.h"
#include "../gameGraphics/OceanRenderingPipeline.h"
#include "../Physics/PhysicsComponents.h"


#include "../gameGraphics/SSAOPipeline.h"
#include "../gameGraphics/CombineSSAOPipeline.h"
#include "../gameGraphics/BlurPipeline.h"

#include "../gameTraps/TrapComponents.h"

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
			typeFilter.addRequirement(components::SkeletonComponent::typeID);

			mInstanceLayout = { 0 };

			mpSkeleton = MeshContainer::GetMeshCPU(GAME_OBJECT_TYPE_UNIT)->GetSkeleton();
			mpSkeleton->StartAnimation(ModelLoader::ANIMATION_TYPE::MOVE);
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
				components::SkeletonComponent* p_skeleton_comp = unit.getComponent<ecs::components::SkeletonComponent>();
				DirectX::XMMATRIX world = UtilityEcsFunctions::GetWorldMatrix(*p_transform_comp);

				XMStoreFloat4x4(&mpBuffer[index].world, world);

				mpBuffer[index].world._44 = PACK(
					p_color_comp->red,
					p_color_comp->green,
					p_color_comp->blue,
					0);

				memcpy(
					mpBuffer[index].boneMatrices,
					p_skeleton_comp->skeletonData.frameData,
					mpSkeleton->jointCount * sizeof(DirectX::XMFLOAT4X4));

				index++;
			}
			mpRenderMgr->SetShaderModelLayout(mRenderProgram, mInstanceLayout);
		}

		void UnitRenderSystem::Initialize(graphics::RenderManager* pRenderMgr, graphics::RenderBuffer* pRenderBuffer)
		{
			mpRenderMgr = pRenderMgr;
			mUnitMeshRegion = MeshContainer::GetMeshGPU(GAME_OBJECT_TYPE_UNIT);

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

#pragma region TileInstanceRenderSystem
		TileInstanceRenderSystem::TileInstanceRenderSystem()
		{
			updateType = SystemUpdateType::MultiEntityUpdate;
			typeFilter.addRequirement(components::TileComponent::typeID);
			typeFilter.addRequirement(components::TransformComponent::typeID);
			typeFilter.addRequirement(components::ColorComponent::typeID);

			mInstanceLayout = { 0 };
		}

		TileInstanceRenderSystem::~TileInstanceRenderSystem()
		{

		}

		void TileInstanceRenderSystem::updateMultipleEntities(EntityIterator& _entities, float _delta)
		{
			mTileCount = (UINT)_entities.entities.size();

			// Fetch pointer to write data to in RenderBuffer
			mpBuffer = (InputLayout*)mpRenderBuffer->GetBufferAddress(mTileCount * GetPerInstanceSize());

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

		void TileInstanceRenderSystem::Initialize(graphics::RenderManager* pRenderMgr, graphics::RenderBuffer* pRenderBuffer)
		{
			mpRenderMgr = pRenderMgr;
			mTileMeshRegion = MeshContainer::GetMeshGPU(GAME_OBJECT_TYPE_TILE);

			mInstanceLayout.MeshCount = 1;
			mInstanceLayout.pMeshes = &mTileMeshRegion;
			mInstanceLayout.pInstanceCountPerMesh = &mTileCount;

			const std::string vs = GetShaderFilepath("VS_Default.cso");
			const std::string ps = GetShaderFilepath("PS_Default.cso");

			mRenderProgram = mpRenderMgr->CreateShaderProgram(
				vs.c_str(),
				ps.c_str(),
				systems::TileInstanceRenderSystem::GetPerInstanceSize());

			mpRenderBuffer = pRenderBuffer;
		}

		uint32_t TileInstanceRenderSystem::GetPerInstanceSize()
		{
			return sizeof(InputLayout);
		}
#pragma endregion TileInstanceRenderSystem

#pragma region OceanInstanceRenderSystem
		OceanInstanceRenderSystem::OceanInstanceRenderSystem()
		{
			updateType = SystemUpdateType::MultiEntityUpdate;
			typeFilter.addRequirement(components::OceanTileComponent::typeID);
			typeFilter.addRequirement(components::TransformComponent::typeID);
			typeFilter.addRequirement(components::ColorComponent::typeID);

			mInstanceLayout = { 0 };
		}

		OceanInstanceRenderSystem::~OceanInstanceRenderSystem()
		{

		}

		void OceanInstanceRenderSystem::updateMultipleEntities(EntityIterator& _entities, float _delta)
		{
			mTileCount = (UINT)_entities.entities.size();

			// Fetch pointer to write data to in RenderBuffer
			mpBuffer = (InputLayout*)mpRenderBuffer->GetBufferAddress(mTileCount * OceanInstanceRenderSystem::GetPerInstanceSize());

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

		void OceanInstanceRenderSystem::Initialize(graphics::RenderManager* pRenderMgr, graphics::RenderBuffer* pRenderBuffer)
		{
			mpRenderMgr = pRenderMgr;
			mTileMeshRegion = MeshContainer::GetMeshGPU(GAME_OBJECT_TYPE_TILE);

			mInstanceLayout.MeshCount = 1;
			mInstanceLayout.pMeshes = &mTileMeshRegion;
			mInstanceLayout.pInstanceCountPerMesh = &mTileCount;

			const std::string vs = GetShaderFilepath("VS_Default.cso");
			const std::string ps = GetShaderFilepath("PS_Default.cso");

			mRenderProgram = mpRenderMgr->CreateShaderProgram(
				vs.c_str(),
				ps.c_str(),
				systems::TileInstanceRenderSystem::GetPerInstanceSize());

			mpRenderBuffer = pRenderBuffer;
		}

		uint32_t OceanInstanceRenderSystem::GetPerInstanceSize()
		{
			return sizeof(InputLayout);
		}
#pragma endregion OceanInstanceRenderSystem

#pragma region OceanRenderSystem
		OceanRenderSystem::OceanRenderSystem()
		{
			updateType = SystemUpdateType::Actor;
			mInstanceLayout = { 0 };
		}

		OceanRenderSystem::~OceanRenderSystem()
		{

		}

		void OceanRenderSystem::act(float _delta)
		{
			UINT index = 0;

			/*
				For now, allocate a way to big buffer size (max size).
				This will be tweaked in later implementations.
			*/
			/*float* height = (float*)malloc(65536);
			ZeroMemory(height, 65536);*/

			/*
				Fetch all ocean tiles and update their y-position in the
				height buffer.
			*/

			for (FilteredEntity& tile : mOceanTiles.entities)
			{
				mpHeightData[index] = tile.getComponent<components::TransformComponent>()->position.y;
				index++;
			}

			/*
				Set our 'vertex buffer' for the world tile mesh, and upload the
				height buffer.
			*/

			graphics::OCEAN_RENDERING_PIPELINE_DATA heightData;
			heightData.pHeightBuffer = mpHeightData;
			heightData.ByteWidth = mHeightDataSize;

			mpRenderMgr->SetShaderModelLayout(mRenderProgram, mInstanceLayout);
			mpStateMgr->UpdatePipelineState(mPipelineState, &heightData);
			mpStateMgr->SetPipelineState(mPipelineState);

			//free(height);
		}

		void OceanRenderSystem::Initialize(
			graphics::RenderManager* pRenderMgr,
			graphics::StateManager* pStateMgr,
			void* pWorldMesh,
			UINT worldMeshVertexCount)
		{
			/*

			*/

			mpRenderMgr = pRenderMgr;
			mpStateMgr = pStateMgr;


			// World is one single mesh
			mInstanceCount = 1;

			mMeshRegion = { 0 };
			mMeshRegion.VertexRegion.Size = worldMeshVertexCount;

			mInstanceLayout.MeshCount = 1;
			mInstanceLayout.pMeshes = &mMeshRegion;
			mInstanceLayout.pInstanceCountPerMesh = &mInstanceCount;

			const std::string vs = GetShaderFilepath("VS_Ocean.cso");
			const std::string ps = GetShaderFilepath("PS_Ocean.cso");

			mRenderProgram = mpRenderMgr->CreateShaderProgram(
				vs.c_str(),
				ps.c_str(),
				0);

			const UINT stride = sizeof(VertexData);
			graphics::OCEAN_RENDERING_PIPELINE_DESC trpDesc;
			trpDesc.pWorldMesh = pWorldMesh;
			trpDesc.stride = stride;
			trpDesc.size = worldMeshVertexCount * stride;

			mPipelineState = mpStateMgr->CreatePipelineState(new graphics::OceanRenderingPipeline(), &trpDesc);
			
			// Grabbing and storing all ocean tiles.
			TypeFilter ocean_filter;
			ocean_filter.addRequirement(components::OceanTileComponent::typeID);
			ocean_filter.addRequirement(components::TransformComponent::typeID);

			mOceanTiles = getEntitiesByFilter(ocean_filter);

			mHeightDataSize = mOceanTiles.entities.size() * sizeof(float);
			mpHeightData = (float*)malloc(mHeightDataSize);
			ZeroMemory(mpHeightData, mHeightDataSize);
		}
#pragma endregion OceanRenderSystem

#pragma region MapRenderSystem
		MapRenderSystem::MapRenderSystem()
		{
			updateType = SystemUpdateType::Actor;
			mInstanceLayout = { 0 };
		}

		MapRenderSystem::~MapRenderSystem()
		{

		}

		void MapRenderSystem::act(float _delta)
		{
			UINT index = 0;

			/*
				For now, allocate a way to big buffer size (max size).
				This will be tweaked in later implementations.
			*/
			/*float* height = (float*)malloc(65536);
			ZeroMemory(height, 65536);*/

			/*
				Fetch all map tiles and update their y-position in the
				height buffer.
			*/

			for (FilteredEntity& tile : mMapTiles.entities)
			{
				if (tile.getComponent<TileComponent>()->tileType == WATER)
				{
					mpHeightData[index] = -1000.f;
				}
				else
				{
					mpHeightData[index] = tile.getComponent<components::TransformComponent>()->position.y;
				}
				index++;
			}

			/*
				Set our 'vertex buffer' for the world tile mesh, and upload the
				height buffer.
			*/

			graphics::TILE_RENDERING_PIPELINE_DATA heightData;
			heightData.pHeightBuffer = mpHeightData;
			heightData.ByteWidth = mHeightDataSize;

			mpRenderMgr->SetShaderModelLayout(mRenderProgram, mInstanceLayout);
			mpStateMgr->UpdatePipelineState(mPipelineState, &heightData);
			mpStateMgr->SetPipelineState(mPipelineState);

			//free(height);
		}

		void MapRenderSystem::Initialize(
			graphics::RenderManager* pRenderMgr,
			graphics::StateManager* pStateMgr,
			void* pWorldMesh,
			UINT worldMeshVertexCount)
		{
			/*

			*/

			mpRenderMgr = pRenderMgr;
			mpStateMgr = pStateMgr;


			// World is one single mesh
			mInstanceCount = 1;

			mMeshRegion = { 0 };
			mMeshRegion.VertexRegion.Size = worldMeshVertexCount;

			mInstanceLayout.MeshCount = 1;
			mInstanceLayout.pMeshes = &mMeshRegion;
			mInstanceLayout.pInstanceCountPerMesh = &mInstanceCount;

			const std::string vs = GetShaderFilepath("VS_Tile.cso");
			const std::string ps = GetShaderFilepath("PS_Default.cso");

			mRenderProgram = mpRenderMgr->CreateShaderProgram(
				vs.c_str(),
				ps.c_str(),
				0);

			const UINT stride = sizeof(VertexData);
			graphics::TILE_RENDERING_PIPELINE_DESC trpDesc;
			trpDesc.pWorldMesh = pWorldMesh;
			trpDesc.stride = stride;
			trpDesc.size = worldMeshVertexCount * stride;

			mPipelineState = mpStateMgr->CreatePipelineState(
				new graphics::TileRenderingPipeline(), &trpDesc);

			// Grabbing and storing all map tiles.
			TypeFilter map_filter;
			map_filter.addRequirement(components::TileComponent::typeID);
			map_filter.addRequirement(components::TransformComponent::typeID);

			mMapTiles = getEntitiesByFilter(map_filter);

			mHeightDataSize = mMapTiles.entities.size() * sizeof(float);
			mpHeightData = (float*)malloc(mHeightDataSize);
			ZeroMemory(mpHeightData, mHeightDataSize);
		}
#pragma endregion MapRenderSystem

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
			//
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
				mObjectTypeCount[p_obj_comp->mObject - SCENE_OBJECT_ENUM_OFFSET]++;
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
				UINT& index = object_type_individual_index[p_obj_comp->mObject - SCENE_OBJECT_ENUM_OFFSET];

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

			for (UINT i = 0; i < SCENE_OBJECT_COUNT; i++)
			{
				mObjectMeshRegion[i] = MeshContainer::GetMeshGPU(SCENE_OBJECT_ENUM_OFFSET + i);
			}

			//mObjectMeshRegion[0] = MeshContainer::GetMeshGPU(GAME_OBJECT_TYPE_BARREL);
			//mObjectMeshRegion[1] = MeshContainer::GetMeshGPU(GAME_OBJECT_TYPE_BOX);
			//mObjectMeshRegion[2] = MeshContainer::GetMeshGPU(GAME_OBJECT_TYPE_CACTUS);

			//mObjectMeshRegion[3] = MeshContainer::GetMeshGPU(GAME_OBJECT_TYPE_CAGE);
			//mObjectMeshRegion[4] = MeshContainer::GetMeshGPU(GAME_OBJECT_TYPE_COWSKULL);
			//mObjectMeshRegion[5] = MeshContainer::GetMeshGPU(GAME_OBJECT_TYPE_FRUITTREE);

			//mObjectMeshRegion[6] = MeshContainer::GetMeshGPU(GAME_OBJECT_TYPE_GIANTSKULL);
			//mObjectMeshRegion[7] = MeshContainer::GetMeshGPU(GAME_OBJECT_TYPE_TOWER);
			//mObjectMeshRegion[8] = MeshContainer::GetMeshGPU(GAME_OBJECT_TYPE_WINTERTREE);

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

#pragma region SSAORenderSystem
		SSAORenderSystem::SSAORenderSystem()
		{
			updateType = Actor;
		}

		SSAORenderSystem::~SSAORenderSystem()
		{
		}

		void SSAORenderSystem::act(float _delta)
		{
			mRenderMgr.ExecutePipeline(
				mPipelineSSAO,
				mShaderSSAO);

			mRenderMgr.ExecutePipeline(
				mPipelineBlur,
				mShaderBlur_v);

			mRenderMgr.ExecutePipeline(
				mPipelineSSAO,
				mShaderBlur);

			mRenderMgr.ExecutePipeline(
				mPipelineCombine,
				mShaderCombine);
		}

		void SSAORenderSystem::Initialize(
			graphics::MeshManager* pMeshMgr,
			const UINT clientWidth,
			const UINT clientHeight)
		{
			mScreenSpaceTriangle = pMeshMgr->CreateMeshRegion(6, 0);
			{
				struct float3
				{
					float x, y, z;
				};

				struct float2
				{
					float x, y;
				};

				float3 vertices[6] =
				{
					-1.0f, -1.0f, 0.5f,
					-1.0f,  1.0f, 0.5f,
					 1.0f, -1.0f, 0.5f,

					 1.0f, -1.0f, 0.5f,
					-1.0f,  1.0f, 0.5f,
					 1.0f,  1.0f, 0.5f,
				};

				float2 uv[6] =
				{
					0.0f, 1.0f,
					0.0f, 0.0f,
					1.0f, 1.0f,

					1.0f, 1.0f,
					0.0f, 0.0f,
					1.0f, 0.0f,
				};

				graphics::VERTEX_DATA data = { NULL };
				data.pVertexPositions = vertices;
				data.pVertexTexCoords = uv;

				pMeshMgr->UploadData(mScreenSpaceTriangle, data, NULL);
			}

			//graphics::RenderManager renderer_ssao;
			mRenderMgr.Initialize(0);

			{
				graphics::SSAO_PIPELINE_DESC desc = { };
				desc.Width = clientWidth / 2.0f;
				desc.Height = clientHeight / 2.0f;
				mPipelineSSAO = mRenderMgr.CreatePipeline(
					new graphics::SSAOPipeline,
					&desc);
			}

			{
				graphics::BLUR_PIPELINE_DESC desc = { };
				desc.Width = clientWidth / 2.0f;
				desc.Height = clientHeight / 2.0f;
				mPipelineBlur = mRenderMgr.CreatePipeline(
					new graphics::BlurPipeline,
					&desc);
			}

			//UINT pipeline_combine;
			{
				graphics::COMBINE_PIPELINE_DESC desc = { };
				desc.Width = clientWidth;
				desc.Height = clientHeight;
				mPipelineCombine = mRenderMgr.CreatePipeline(
					new graphics::CombinePipeline,
					&desc);
			}

			mShaderSSAO = mRenderMgr.CreateShaderProgram(
				GetShaderFilepath("VS_SSAO.cso").c_str(),
				GetShaderFilepath("PS_SSAO.cso").c_str(),
				0);

			mShaderBlur = mRenderMgr.CreateShaderProgram(
				GetShaderFilepath("VS_SSAO.cso").c_str(),
				GetShaderFilepath("PS_Blur_Horizontal.cso").c_str(),
				0);

			mShaderBlur_v = mRenderMgr.CreateShaderProgram(
				GetShaderFilepath("VS_SSAO.cso").c_str(),
				GetShaderFilepath("PS_Blur_Vertical.cso").c_str(),
				0);

			mShaderCombine = mRenderMgr.CreateShaderProgram(
				GetShaderFilepath("VS_SSAO.cso").c_str(),
				GetShaderFilepath("PS_Combine.cso").c_str(),
				0);


			mObjectCount = 1;
			mInstanceLayout.MeshCount = 1;
			mInstanceLayout.pInstanceCountPerMesh = &mObjectCount;
			mInstanceLayout.pMeshes = &mScreenSpaceTriangle;

			mRenderMgr.SetShaderModelLayout(mShaderSSAO, mInstanceLayout);
			mRenderMgr.SetShaderModelLayout(mShaderBlur, mInstanceLayout);
			mRenderMgr.SetShaderModelLayout(mShaderBlur_v, mInstanceLayout);
			mRenderMgr.SetShaderModelLayout(mShaderCombine, mInstanceLayout);
		}
#pragma endregion SSAORenderSystem

#pragma region WeaponRenderSystem
		WeaponRenderSystem::WeaponRenderSystem()
		{
			updateType = SystemUpdateType::MultiEntityUpdate;
			typeFilter.addRequirement(components::WeaponComponent::typeID);
			typeFilter.addRequirement(components::TransformComponent::typeID);
			typeFilter.addRequirement(components::ColorComponent::typeID);
		}

		WeaponRenderSystem::~WeaponRenderSystem()
		{
			//
		}

		void WeaponRenderSystem::updateMultipleEntities(EntityIterator& _entities, float _delta)
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
			mpBuffer = (InputLayout*)mpRenderBuffer->GetBufferAddress(mObjectCount * systems::WeaponRenderSystem::GetPerInstanceSize());

			// Count how many instances we have per scene object mesh
			ZeroMemory(mObjectTypeCount, WEAPON_COUNT * sizeof(UINT));
			for (FilteredEntity object : _entities.entities)
			{
				components::WeaponComponent* p_obj_comp = object.getComponent<components::WeaponComponent>();
				mObjectTypeCount[p_obj_comp->mType - (GAME_OBJECT_TYPE_WEAPON_OFFSET_TAG + 1)]++;
			}

			// Set index to write to in RenderBuffer, per mesh
			UINT object_type_individual_index[WEAPON_COUNT] = { 0 };

			for (int i = 1; i < WEAPON_COUNT; i++)
			{
				object_type_individual_index[i] = object_type_individual_index[i - 1] + mObjectTypeCount[i - 1];
			}

			// Iterate all objects and write their data to the RenderBuffer
			for (FilteredEntity weapon : _entities.entities)
			{
				components::WeaponComponent* p_weapon_comp = weapon.getComponent<components::WeaponComponent>();

				// Expection on weapon types: fist for example does not have a mesh to render.
				if (p_weapon_comp->mType == GAME_OBJECT_TYPE_WEAPON_FIST)
				{
					continue;
				}

				// Get index, depending on mesh type
				UINT& index = object_type_individual_index[p_weapon_comp->mType - (GAME_OBJECT_TYPE_WEAPON_OFFSET_TAG + 1)];

				PlaceWorldMatrix(weapon, mpBuffer[index].world);
				index++;
			}

			mInstanceLayout.pInstanceCountPerMesh = mObjectTypeCount;
			mpRenderMgr->SetShaderModelLayout(mRenderProgram, mInstanceLayout);
		}

		void WeaponRenderSystem::Initialize(graphics::RenderManager* pRenderMgr, graphics::RenderBuffer* pRenderBuffer)
		{
			mpRenderMgr = pRenderMgr;

			/*
				Set up mesh region for all meshes that will be rendered.
			*/
			for (int i = 0; i < WEAPON_COUNT; i++)
			{
				mObjectMeshRegion[i] = MeshContainer::GetMeshGPU(GAME_OBJECT_TYPE(i + (GAME_OBJECT_TYPE_WEAPON_OFFSET_TAG + 1)));
			}

			mInstanceLayout.MeshCount = WEAPON_COUNT;
			mInstanceLayout.pMeshes = mObjectMeshRegion;
			mInstanceLayout.pInstanceCountPerMesh = &mObjectCount;

			const std::string vs = GetShaderFilepath("VS_Weapon.cso");
			const std::string ps = GetShaderFilepath("PS_Default.cso");

			mRenderProgram = mpRenderMgr->CreateShaderProgram(
				vs.c_str(),
				ps.c_str(),
				systems::WeaponRenderSystem::GetPerInstanceSize());

			mpRenderBuffer = pRenderBuffer;
		}

		uint32_t WeaponRenderSystem::GetPerInstanceSize()
		{
			return sizeof(WeaponRenderSystem::InputLayout);
		}

		void WeaponRenderSystem::PlaceWorldMatrix(FilteredEntity& rWeapon, DirectX::XMFLOAT4X4& rDestination)
		{
			components::WeaponComponent* p_weapon_comp = rWeapon.getComponent<components::WeaponComponent>();
			components::TransformComponent* p_transform_comp = rWeapon.getComponent<components::TransformComponent>();

			if (!p_weapon_comp->mOwnerEntity)
			{
				/*
					If weapon don't have an owner, use the weapon's transform.
				*/

				XMStoreFloat4x4(&rDestination, UtilityEcsFunctions::GetWorldMatrix(*p_transform_comp));
				return;
			}

			/*
				Calculate position of owner entity's hand.
			*/

			// Overwrite transform component by by transform of owner entity.
			p_transform_comp = getComponentFromKnownEntity<TransformComponent>(p_weapon_comp->mOwnerEntity);

			SkeletonComponent* p_skeleton = getComponentFromKnownEntity<SkeletonComponent>(p_weapon_comp->mOwnerEntity);
			XMFLOAT4X4 right_hand_offset_matrix = p_skeleton->skeletonData.GetOffsetMatrixUsingJointName("Hand.r");

			// Hand position in model space.
			XMFLOAT3 origin_to_hand = ORIGIN_TO_HAND;
			XMMATRIX hand_trans = XMMatrixTranslationFromVector(XMLoadFloat3(&origin_to_hand));

			// Final world transform.
			XMMATRIX world = hand_trans * XMMatrixTranspose(XMLoadFloat4x4(&right_hand_offset_matrix)) * UtilityEcsFunctions::GetWorldMatrix(*p_transform_comp);

			XMStoreFloat4x4(&rDestination, world);
		}
#pragma endregion WeaponRenderSystem

#pragma region TrapRenderSystem
		TrapRenderSystem::TrapRenderSystem()
		{
			updateType = SystemUpdateType::MultiEntityUpdate;
			typeFilter.addRequirement(components::TrapComponent::typeID);
			typeFilter.addRequirement(components::TransformComponent::typeID);
			typeFilter.addRequirement(components::ColorComponent::typeID);
		}

		TrapRenderSystem::~TrapRenderSystem()
		{
			//
		}

		void TrapRenderSystem::updateMultipleEntities(EntityIterator& _entities, float _delta)
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

			mObjectCount = _entities.entities.size() * 2;

			// Fetch pointer to write data to in RenderBuffer
			mpBuffer = (InputLayout*)mpRenderBuffer->GetBufferAddress(mObjectCount * systems::TrapRenderSystem::GetPerInstanceSize());

			// Count how many instances we have per scene object mesh
			ZeroMemory(mObjectTypeCount, TRAP_COUNT * sizeof(UINT));
			for (FilteredEntity object : _entities.entities)
			{
				components::TrapComponent* p_obj_comp = object.getComponent<components::TrapComponent>();
				mObjectTypeCount[p_obj_comp->mObjectType - (GAME_OBJECT_TYPE_TRAP_OFFSET_TAG + 1)]++;
				mObjectTypeCount[p_obj_comp->mObjectType - (GAME_OBJECT_TYPE_TRAP_OFFSET_TAG + 1)]++;
			}

			// Set index to write to in RenderBuffer, per mesh
			UINT object_type_individual_index[TRAP_COUNT] = { 0 };

			for (int i = 1; i < TRAP_COUNT; i++)
			{
				object_type_individual_index[i] = object_type_individual_index[i - 1] + mObjectTypeCount[i - 1];
			}

			// Iterate all objects and write their data to the RenderBuffer
			for (FilteredEntity trap : _entities.entities)
			{
				components::TrapComponent* p_trap_comp = trap.getComponent<components::TrapComponent>();
				components::ColorComponent* p_color_comp = trap.getComponent<components::ColorComponent>();
				components::TransformComponent* p_transform_comp = trap.getComponent<components::TransformComponent>();

				// Get index, depending on mesh type
				UINT& index = object_type_individual_index[p_trap_comp->mObjectType - (GAME_OBJECT_TYPE_TRAP_OFFSET_TAG + 1)];

				/*
					Create a world matrix out of the trap's transform.
					Place color of trap in the last element of the world
					matrix. Color will be extracted in the shader.
				*/

				components::TransformComponent outline_transform = *p_transform_comp;
				outline_transform.position.y -= 0.002f;
				outline_transform.scale = { 1.f, 1.f, 1.f };

				XMStoreFloat4x4(&mpBuffer[index].world, UtilityEcsFunctions::GetWorldMatrix(outline_transform));
				mpBuffer[index].world._44 = PACK(0, 0, 0, 255);

				index++;

				XMStoreFloat4x4(&mpBuffer[index].world, UtilityEcsFunctions::GetWorldMatrix(*p_transform_comp));
				mpBuffer[index].world._44 = PACK(p_color_comp->red, p_color_comp->green, p_color_comp->blue, 255);

				index++;
			}

			mInstanceLayout.pInstanceCountPerMesh = mObjectTypeCount;
			mpRenderMgr->SetShaderModelLayout(mRenderProgram, mInstanceLayout);
		}

		void TrapRenderSystem::Initialize(graphics::RenderManager* pRenderMgr, graphics::RenderBuffer* pRenderBuffer)
		{
			mpRenderMgr = pRenderMgr;

			/*
				Set up mesh region for all meshes that will be rendered.
			*/
			for (int i = 0; i < TRAP_COUNT; i++)
			{
				mObjectMeshRegion[i] = MeshContainer::GetMeshGPU(GAME_OBJECT_TYPE(i + (GAME_OBJECT_TYPE_TRAP_OFFSET_TAG + 1)));
			}

			mInstanceLayout.MeshCount = TRAP_COUNT;
			mInstanceLayout.pMeshes = mObjectMeshRegion;
			mInstanceLayout.pInstanceCountPerMesh = &mObjectCount;

			const std::string vs = GetShaderFilepath("VS_Trap.cso");
			const std::string ps = GetShaderFilepath("PS_Ocean.cso");

			mRenderProgram = mpRenderMgr->CreateShaderProgram(
				vs.c_str(),
				ps.c_str(),
				systems::TrapRenderSystem::GetPerInstanceSize());

			mpRenderBuffer = pRenderBuffer;
		}

		uint32_t TrapRenderSystem::GetPerInstanceSize()
		{
			return sizeof(TrapRenderSystem::InputLayout);
		}
#pragma endregion TrapRenderSystem
	}
}