#include "Renderers.h"
#include "../gameAI/AIComponents.h"
#include "../gameUtility/UtilityComponents.h"
#include "../gameUtility/UtilityEcsFunctions.h"
#include "../gameUtility/UtilityGraphics.h"
#include "../gameSceneObjects/SceneObjectComponents.h"
#include "../gameWorld/OceanComponents.h"
#include "../gameAnimation/AnimationComponents.h"

#include "../gameGraphics/TileRenderingPipeline.h"
#include "../Physics/PhysicsComponents.h"

#include "../gameEcs/GameWeaponComponents.h"

#include "../gameGraphics/SSAOPipeline.h"
#include "../gameGraphics/CombineSSAOPipeline.h"
#include "../gameGraphics/BlurPipeline.h"



void WeaponRenderer::InitializeInternal(
	graphics::RenderManager* pRenderMgr,
	graphics::RenderBuffer* pRenderBuffer,
	GAME_OBJECT_TYPE objectToRender)
{
	mpRenderMgr = pRenderMgr;
	mpRenderBuffer = pRenderBuffer;
	mWeaponMeshRegion = MeshContainer::GetMeshGPU(objectToRender);

	mInstanceLayout.MeshCount = 1;
	mInstanceLayout.pMeshes = &mWeaponMeshRegion;
	mInstanceLayout.pInstanceCountPerMesh = &mInstanceCount;

	const std::string vs = GetShaderFilepath("VS_Weapon.cso");
	const std::string ps = GetShaderFilepath("PS_Default.cso");

	mRenderProgram = mpRenderMgr->CreateShaderProgram(
		vs.c_str(),
		ps.c_str(),
		sizeof(InstanceData));
}

void WeaponRenderer::RenderAllInternal(ecs::EntityIterator& weaponEntities)
{
	mInstanceCount = (UINT)weaponEntities.entities.size();

	// Fetch pointer to write data to in RenderBuffer
	mpInstanceDataBuffer = (InstanceData*)mpRenderBuffer->GetBufferAddress(mInstanceCount * sizeof(InstanceData));

	// Iterate all tiles and write their data to the RenderBuffer
	uint32_t index = 0;
	for (ecs::FilteredEntity weapon : weaponEntities.entities)
	{
		ecs::components::WeaponComponent* p_weapon_comp = weapon.getComponent<ecs::components::WeaponComponent>();
		ecs::components::TransformComponent* p_transform_comp = weapon.getComponent<ecs::components::TransformComponent>();

		/*
			If weapon is held by a unit, retrieve the unit's hand location and adjust the weapon
			position to it.

			If not held by any unit, just render the weapon on the ground.
		*/
		if (p_weapon_comp->mOwnerEntity != 0)
		{
			p_transform_comp = getComponentFromKnownEntity<TransformComponent>(p_weapon_comp->mOwnerEntity);
			ecs::Entity* owner = getEntity(p_weapon_comp->mOwnerEntity);
			if (!owner->hasComponentOfType(SkeletonComponent::typeID))
			{
				continue;
			}
			SkeletonComponent* p_skeleton = getComponentFromKnownEntity<SkeletonComponent>(p_weapon_comp->mOwnerEntity);
			XMFLOAT4X4 right_hand_offset_matrix = p_skeleton->skeletonData.GetOffsetMatrixUsingJointName("Hand.r");

			// Hand position in model space.
			XMFLOAT3 origin_to_hand = ORIGIN_TO_HAND;
			XMMATRIX hand_trans = XMMatrixTranslationFromVector(XMLoadFloat3(&origin_to_hand));

			// Final world transform.
			XMMATRIX world = hand_trans * XMMatrixTranspose(XMLoadFloat4x4(&right_hand_offset_matrix)) * UtilityEcsFunctions::GetWorldMatrix(*p_transform_comp);

			XMStoreFloat4x4(&mpInstanceDataBuffer[index].world, world);
		}
		else
		{
			XMStoreFloat4x4(&mpInstanceDataBuffer[index].world, UtilityEcsFunctions::GetWorldMatrix(*p_transform_comp));
		}

		index++;
	}

	mpRenderMgr->SetShaderModelLayout(mRenderProgram, mInstanceLayout);
}







namespace ecs
{
	namespace systems
	{
#pragma region WeaponRenderSystems

		DEFINE_WEAPON_RENDERER(SwordRenderSystem, SwordComponent, GAME_OBJECT_TYPE_SWORD)

#pragma endregion WeaponRenderSystems

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

		void TileRenderSystem::Initialize(graphics::RenderManager* pRenderMgr, graphics::RenderBuffer* pRenderBuffer)
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
			mTileMeshRegion = MeshContainer::GetMeshGPU(GAME_OBJECT_TYPE_TILE);

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

#pragma region WorldRenderSystem
		WorldRenderSystem::WorldRenderSystem()
		{
			updateType = SystemUpdateType::Actor;
			mInstanceLayout = { 0 };
		}

		WorldRenderSystem::~WorldRenderSystem()
		{

		}

		void WorldRenderSystem::act(float _delta)
		{
			UINT index = 0;

			/*
				For now, allocate a way to big buffer size (max size).
				This will be tweaked in later implementations.
			*/
			float* height = (float*)malloc(65536);
			ZeroMemory(height, 65536);

			/*
				Fetch all ocean tiles and update their y-position in the
				height buffer.
			*/

			EntityIterator p_iterator = mOceanTiles;

			for (FilteredEntity& tile : p_iterator.entities)
			{
				height[index] = tile.getComponent<components::TransformComponent>()->position.y;
				index++;
			}

			/*
				Fetch all map tiles and update their y-position in the
				height buffer.
			*/

			p_iterator = mMapTiles;

			for (FilteredEntity& tile : p_iterator.entities)
			{
				height[index] = tile.getComponent<components::TransformComponent>()->position.y;
				index++;
			}

			/*
				Set our 'vertex buffer' for the world tile mesh, and upload the
				height buffer.
			*/

			graphics::TILE_RENDERING_PIPELINE_DATA heightData;
			heightData.pHeightBuffer = height;
			heightData.ByteWidth = index * sizeof(float);

			mpRenderMgr->SetShaderModelLayout(mRenderProgram, mInstanceLayout);
			mpStateMgr->UpdatePipelineState(mPipelineState, &heightData);
			mpStateMgr->SetPipelineState(mPipelineState);

			free(height);
		}

		void WorldRenderSystem::Initialize(
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

			mPipelineState = mpStateMgr->CreatePipelineState(new graphics::TileRenderingPipeline(), &trpDesc);
			
			// Grabbing and storing all ocean tiles.
			TypeFilter ocean_filter;
			ocean_filter.addRequirement(components::OceanTileComponent::typeID);
			ocean_filter.addRequirement(components::TransformComponent::typeID);

			mOceanTiles = getEntitiesByFilter(ocean_filter);

			// Grabbing and storing all map tiles.
			TypeFilter map_filter;
			map_filter.addRequirement(components::TileComponent::typeID);
			map_filter.addRequirement(components::TransformComponent::typeID);

			mMapTiles = getEntitiesByFilter(map_filter);
		}
#pragma endregion WorldRenderSystem

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

			mObjectMeshRegion[0] = MeshContainer::GetMeshGPU(GAME_OBJECT_TYPE_BARREL);
			mObjectMeshRegion[1] = MeshContainer::GetMeshGPU(GAME_OBJECT_TYPE_BOX);
			mObjectMeshRegion[2] = MeshContainer::GetMeshGPU(GAME_OBJECT_TYPE_CACTUS);

			mObjectMeshRegion[3] = MeshContainer::GetMeshGPU(GAME_OBJECT_TYPE_CAGE);
			mObjectMeshRegion[4] = MeshContainer::GetMeshGPU(GAME_OBJECT_TYPE_COWSKULL);
			mObjectMeshRegion[5] = MeshContainer::GetMeshGPU(GAME_OBJECT_TYPE_FRUITTREE);

			mObjectMeshRegion[6] = MeshContainer::GetMeshGPU(GAME_OBJECT_TYPE_GIANTSKULL);
			mObjectMeshRegion[7] = MeshContainer::GetMeshGPU(GAME_OBJECT_TYPE_TOWER);
			mObjectMeshRegion[8] = MeshContainer::GetMeshGPU(GAME_OBJECT_TYPE_WINTERTREE);

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
				mShaderBlur);

			mRenderMgr.ExecutePipeline(
				mPipelineSSAO,
				mShaderBlur_v);

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
	}
}