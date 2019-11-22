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
#include "../gameGraphics/GraphicsECSComponents.h"
#include "../gameGraphics/ParticleECSComponents.h"


#include "../gameGraphics/SSAOPipeline.h"
#include "../gameGraphics/CombineSSAOPipeline.h"
#include "../gameGraphics/BlurPipeline.h"

#include "../gameTraps/TrapComponents.h"

#include "../gameGraphics/FakeStencilPipeline.h"


// Normalizes scene objects and scene meshes to allow for indexing in array
#define TO_MESH(x) (x - GAME_OBJECT_TYPE_MESH_START)
#define TO_SCENE(x) (x - SCENE_OBJECT_ENUM_OFFSET) 

//#define RUN_SSAO	// Comment this out if performance is unbearable

#include "../gamePowerups/PowerupComponents.h"

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

				// This world matrix value is being hijacked for rendering the "fake stencil" for outlines
				// The defining army color value is stored in the alpha and used in the outline shaders
				mpBuffer[index].world._34 = (uint32_t)p_color_comp->alpha;

				mpBuffer[index].world._44 = PACK(
					p_color_comp->red,
					p_color_comp->green,
					p_color_comp->blue,
					p_color_comp->alpha);

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

#pragma region ParticleRenderSystem
		ParticleRenderSystem::ParticleRenderSystem()
		{
			mInstanceLayout = { 0 };
			updateType = SystemUpdateType::MultiEntityUpdate;
			typeFilter.addRequirement(components::ParticleComponent::typeID);

		}

		ParticleRenderSystem::~ParticleRenderSystem()
		{

		}

		void ParticleRenderSystem::updateMultipleEntities(EntityIterator& _entities, float _delta)
		{
			mParticleCount = (UINT)_entities.entities.size();

			// Fetch pointer to write data to in RenderBuffer
			mpBuffer = (InputLayout*)mpRenderBuffer->GetBufferAddress(mParticleCount * GetPerInstanceSize());

			uint32_t index = 0;
			for (FilteredEntity tile : _entities.entities)
			{
				components::ParticleComponent* p_particle_comp = tile.getComponent<components::ParticleComponent>();

				mpBuffer[index].x = p_particle_comp->Position.x;
				mpBuffer[index].y = p_particle_comp->Position.y;
				mpBuffer[index].z = p_particle_comp->Position.z;

				mpBuffer[index].color = PACK(
					p_particle_comp->Red, 
					p_particle_comp->Green, 
					p_particle_comp->Blue, 
					p_particle_comp->Scale
				);

				index++;
			}

			mpRenderMgr->SetShaderModelLayout(mRenderProgram, mInstanceLayout);
		}

		void ParticleRenderSystem::Initialize(
			graphics::RenderManager* pRenderMgr, 
			graphics::RenderBuffer* pRenderBuffer,
			graphics::StateManager* pStateMgr)
		{
			mpRenderMgr		= pRenderMgr;
			mpStateMgr		= pStateMgr;
			mpRenderBuffer	= pRenderBuffer;

			mParticleMeshRegion = MeshContainer::GetMeshGPU(GAME_OBJECT_TYPE_QUAD);

			mInstanceLayout.MeshCount = 1;
			mInstanceLayout.pMeshes = &mParticleMeshRegion;
			mInstanceLayout.pInstanceCountPerMesh = &mParticleCount;

			const std::string vs = GetShaderFilepath("VS_Particle.cso");
			const std::string ps = GetShaderFilepath("PS_Particle.cso");

			mRenderProgram = mpRenderMgr->CreateShaderProgram(
				vs.c_str(),
				ps.c_str(),
				systems::ParticleRenderSystem::GetPerInstanceSize());

			//mStatePipeline = mpStateMgr->CreatePipelineState(NULL, NULL);
		}

		uint32_t ParticleRenderSystem::GetPerInstanceSize()
		{
			return sizeof(InputLayout);
		}
#pragma endregion ParticleRenderSystem

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

				mpBuffer[index].color = PACK(
					p_color_comp->red, 
					p_color_comp->green, 
					p_color_comp->blue, 
					0);

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


			// Count how many instances we have per scene object mesh
			ZeroMemory(mInstancePerMesh, GAME_OBJECT_TYPE_MESH_COUNT * sizeof(UINT));
			for (FilteredEntity object : _entities.entities)
			{
				components::SceneObjectComponent* p_obj_comp = object.getComponent<components::SceneObjectComponent>();

				const UINT scene_object_index = TO_SCENE(p_obj_comp->mObject);

				for (UINT i = mMap[scene_object_index].Start; i <= mMap[scene_object_index].End; i++)
				{
					mInstancePerMesh[TO_MESH(i)]++;
				}
			}
			 
			mObjectCount = 0; // Sum it up
			for (UINT i = 0; i < GAME_OBJECT_TYPE_MESH_COUNT; i++)
			{
				mObjectCount += mInstancePerMesh[i];
			}

			// Fetch pointer to write data to in RenderBuffer
			mpBuffer = (InputLayout*)mpRenderBuffer->GetBufferAddress(mObjectCount * systems::SceneObjectRenderSystem::GetPerInstanceSize());


			// Set index to write to in RenderBuffer, per mesh
			UINT object_type_individual_index[GAME_OBJECT_TYPE_MESH_COUNT] = { 0 };

			for (int i = 1; i < GAME_OBJECT_TYPE_MESH_COUNT; i++)
			{
				object_type_individual_index[i] = object_type_individual_index[i - 1] + mInstancePerMesh[i - 1];
			}

			// Iterate all objects and write their data to the RenderBuffer
			for (FilteredEntity object : _entities.entities)
			{
				components::SceneObjectComponent* p_obj_comp = object.getComponent<components::SceneObjectComponent>();
				components::TransformComponent* p_transform_comp = object.getComponent<components::TransformComponent>();

				const UINT scene_object_index = TO_SCENE(p_obj_comp->mObject);

				// For each scene object add their respective scene mesh for rendering
				for (UINT i = mMap[scene_object_index].Start; i <= mMap[scene_object_index].End; i++)
				{
					UINT& index = object_type_individual_index[TO_MESH(i)];

					DirectX::XMMATRIX world = UtilityEcsFunctions::GetWorldMatrix(*p_transform_comp);

					XMStoreFloat4x4(&mpBuffer[index].world, world);

					const uint3 color = mColors[TO_MESH(i)];
					mpBuffer[index].world._44 = PACK(color.Red, color.Green, color.Blue, 0);

					index++;
				}
			}

			mInstanceLayout.pInstanceCountPerMesh = mInstancePerMesh;
			mpRenderMgr->SetShaderModelLayout(mRenderProgram, mInstanceLayout);
		}

		void SceneObjectRenderSystem::Initialize(graphics::RenderManager* pRenderMgr, graphics::RenderBuffer* pRenderBuffer)
		{
			mpRenderMgr = pRenderMgr;

			for (UINT i = 0; i < GAME_OBJECT_TYPE_MESH_COUNT - 1; i++) //Don't include error
			{
				const UINT index = i + GAME_OBJECT_TYPE_MESH_START;
				mObjectMeshRegion[i] = MeshContainer::GetMeshGPU(index);
			}

			mObjectMeshRegion[TO_MESH(GAME_OBJECT_TYPE_MESH_ERROR)]
				= MeshContainer::GetMeshGPU(GAME_OBJECT_TYPE_FRUITTREE);

			mInstanceLayout.MeshCount				= GAME_OBJECT_TYPE_MESH_COUNT;
			mInstanceLayout.pMeshes					= mObjectMeshRegion;
			mInstanceLayout.pInstanceCountPerMesh	= &mObjectCount;

			/*
				Add new meshes here

				Use 'mMap' to map scene object to scene mesh
				Use 'mColors' to apply color to each mesh (colors will not differ between same scene object)
			*/

			/* --- Fruit Tree --- */
			mMap[TO_SCENE(GAME_OBJECT_TYPE_FRUITTREE)] = { 
				GAME_OBJECT_TYPE_TREE_LEAVES,
				GAME_OBJECT_TYPE_TREE_ROCK 
			};

			mColors[TO_MESH(GAME_OBJECT_TYPE_TREE_LEAVES)]	= { 104, 200, 59 };
			mColors[TO_MESH(GAME_OBJECT_TYPE_TREE_TRUNK)]	= { 104,  72, 59 };
			mColors[TO_MESH(GAME_OBJECT_TYPE_TREE_ROCK)]	= {  60,  60, 60 };
			

			/* -- Barrel --- */
			mMap[TO_SCENE(GAME_OBJECT_TYPE_BARREL)] = { 
				GAME_OBJECT_TYPE_BARREL_STONES, 
				GAME_OBJECT_TYPE_BARREL_BARREL 
			};

			mColors[TO_MESH(GAME_OBJECT_TYPE_BARREL_STONES)] = {  60,  60, 60 };
			mColors[TO_MESH(GAME_OBJECT_TYPE_BARREL_BARREL)] = { 104,  72, 59 };


			/* -- Winter Tree --- */
			mMap[TO_SCENE(GAME_OBJECT_TYPE_WINTERTREE)] = {
				GAME_OBJECT_TYPE_PINE_LEAVES,
				GAME_OBJECT_TYPE_PINE_TRUNK
			};

			mColors[TO_MESH(GAME_OBJECT_TYPE_PINE_LEAVES)]	= { 200, 200, 200 };
			mColors[TO_MESH(GAME_OBJECT_TYPE_PINE_TRUNK)]	= { 104, 72, 59 };


			/* -- Cactus --- */
			mMap[TO_SCENE(GAME_OBJECT_TYPE_CACTUS)] = {
				GAME_OBJECT_TYPE_DESERT_CACTUS,
				GAME_OBJECT_TYPE_DESERT_SKULL
			};

			mColors[TO_MESH(GAME_OBJECT_TYPE_DESERT_CACTUS)] = { 104, 200, 59 };
			mColors[TO_MESH(GAME_OBJECT_TYPE_DESERT_BOX)] = { 104, 72, 59 };
			mColors[TO_MESH(GAME_OBJECT_TYPE_DESERT_SKULL)] = { 160, 150, 140 };


			/* -- Tower --- */
			mMap[TO_SCENE(GAME_OBJECT_TYPE_TOWER)] = {
				GAME_OBJECT_TYPE_TOWER_TOWER,
				GAME_OBJECT_TYPE_TOWER_CAGE
			};

			mColors[TO_MESH(GAME_OBJECT_TYPE_TOWER_TOWER)] = { 60,  60, 60 };
			mColors[TO_MESH(GAME_OBJECT_TYPE_TOWER_CAGE)] = { 60,  60, 60 };

			/* -- Giant Skull --- */
			mMap[TO_SCENE(GAME_OBJECT_TYPE_GIANTSKULL)] = {
				GAME_OBJECT_TYPE_MESH_GIANTSKULL,
				GAME_OBJECT_TYPE_MESH_GIANTSKULL
			};

			mColors[TO_MESH(GAME_OBJECT_TYPE_MESH_GIANTSKULL)] = { 160, 150, 140 };

			/* -- Box --- */
			mMap[TO_SCENE(GAME_OBJECT_TYPE_BOX)] = {
				GAME_OBJECT_TYPE_MESH_BOX,
				GAME_OBJECT_TYPE_MESH_BOX
			};

			mColors[TO_MESH(GAME_OBJECT_TYPE_MESH_BOX)] = { 104, 72, 59 };

			/* -- Cow Skull --- */
			mMap[TO_SCENE(GAME_OBJECT_TYPE_COWSKULL)] = {
				GAME_OBJECT_TYPE_MESH_COWSKULL,
				GAME_OBJECT_TYPE_MESH_COWSKULL
			};

			mColors[TO_MESH(GAME_OBJECT_TYPE_MESH_COWSKULL)] = { 160, 150, 140 };

			/* -- Cage --- */
			mMap[TO_SCENE(GAME_OBJECT_TYPE_CAGE)] = {
				GAME_OBJECT_TYPE_MESH_CAGE,
				GAME_OBJECT_TYPE_MESH_CAGE
			};

			mColors[TO_MESH(GAME_OBJECT_TYPE_MESH_CAGE)] = { 60,  60, 60 };

			// End

			const std::string vs = GetShaderFilepath("VS_Weapon.cso");
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
			mRenderMgr.Destroy();
		}

		void SSAORenderSystem::act(float _delta)
		{
#ifdef RUN_SSAO
			mRenderMgr.ExecutePipeline(
				mPipelineSSAO,
				mShaderSSAO);

			mRenderMgr.ExecutePipeline(
				mPipelineBlur,
				mShaderBlur);

			mRenderMgr.ExecutePipeline(
				mPipelineSSAO,
				mShaderBlur_v);
#endif // !RUN_SSAO

			mRenderMgr.ExecutePipeline(
				mPipelineCombine,
				mShaderCombine);
		}

		void SSAORenderSystem::Initialize(
			const UINT clientWidth,
			const UINT clientHeight)
		{
			mScreenSpaceTriangle = MeshContainer::GetMeshGPU(GAME_OBJECT_TYPE_QUAD);
			mRenderMgr.Initialize(0);

			const UINT width = clientWidth;
			const UINT height = clientHeight;

			{
				graphics::SSAO_PIPELINE_DESC desc = { };
				desc.Width = width;
				desc.Height = height;
				mPipelineSSAO = mRenderMgr.CreatePipeline(
					new graphics::SSAOPipeline,
					&desc);
			}

			{
				graphics::BLUR_PIPELINE_DESC desc = { };
				desc.Width = width;
				desc.Height = height;
				mPipelineBlur = mRenderMgr.CreatePipeline(
					new graphics::BlurPipeline,
					&desc);
			}

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

#pragma region OutlineRenderSystem
		OutlineRenderSystem::OutlineRenderSystem()
		{
			updateType = Actor;
		}
		OutlineRenderSystem::~OutlineRenderSystem()
		{
			// Borrowing the rendermgr from UnitRenderSystem so don't destroy
			//mRenderMgr.Destroy();
		}
		void OutlineRenderSystem::act(float _delta)
		{

			mRenderMgr->ExecutePipeline(mPipelineFakeStencil, this->unitRenderProgram);
		}
		void OutlineRenderSystem::Initialize(const UINT clientWidth, const UINT clientHeight,
			const UINT unitRenderProgram, graphics::RenderManager* unitRenderManager)
		{
			// This render system uses the render manager of the regular Unit render system but with
			// different shaders


			this->unitRenderProgram = unitRenderProgram;
			this->mRenderMgr = unitRenderManager;
			{
				graphics::FAKE_STENCIL_PIPELINE_DESC fake_stencil_desc = { };
				fake_stencil_desc.ClientWidth = clientWidth;
				fake_stencil_desc.ClientHeight = clientHeight;
				graphics::FakeStencilPipeline* fake_stencil_pipeline = new graphics::FakeStencilPipeline;
				this->mPipelineFakeStencil = mRenderMgr->CreatePipeline(
					fake_stencil_pipeline,
					&fake_stencil_desc);

			}

			
		}
#pragma endregion OutlineRenderSystem

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
			ZeroMemory(mObjectTypeCount, WEAPON_TYPE_COUNT * sizeof(UINT));
			for (FilteredEntity object : _entities.entities)
			{
				components::WeaponComponent* p_obj_comp = object.getComponent<components::WeaponComponent>();
				mObjectTypeCount[p_obj_comp->mType - (GAME_OBJECT_TYPE_WEAPON_OFFSET_TAG + 1)]++;
			}

			// Set index to write to in RenderBuffer, per mesh
			UINT object_type_individual_index[WEAPON_TYPE_COUNT] = { 0 };

			for (int i = 1; i < WEAPON_TYPE_COUNT; i++)
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
			for (int i = 0; i < WEAPON_TYPE_COUNT; i++)
			{
				mObjectMeshRegion[i] = MeshContainer::GetMeshGPU(GAME_OBJECT_TYPE(i + (GAME_OBJECT_TYPE_WEAPON_OFFSET_TAG + 1)));
			}

			mInstanceLayout.MeshCount = WEAPON_TYPE_COUNT;
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
			components::ColorComponent* p_color_comp = rWeapon.getComponent<components::ColorComponent>();
				
			if (!p_weapon_comp->mOwnerEntity)
			{
				/*
					If weapon don't have an owner, use the weapon's transform.
				*/

				XMStoreFloat4x4(&rDestination, UtilityEcsFunctions::GetWorldMatrix(*p_transform_comp));
			}
			else
			{
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

			/*
				-- Set color
				Since we have to align instance data, we use the last element of the 
				world matrix to store color data. This is extraxted later in the
				vertex shader.
			*/
			rDestination._44 = PACK(p_color_comp->red, p_color_comp->green, p_color_comp->blue, 255);
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

				When rendering traps specific, we create two meshes for each trap. The first is the 
				trap itself, and the other one is the same mesh but larger and black in color. This
				gives a simple mesh "outline" for viewing meshes during development.
			*/

			mObjectCount = _entities.entities.size() * 2;

			// Fetch pointer to write data to in RenderBuffer
			mpBuffer = (InputLayout*)mpRenderBuffer->GetBufferAddress(mObjectCount * systems::TrapRenderSystem::GetPerInstanceSize());

			// Count how many instances we have per scene object mesh
			ZeroMemory(mObjectTypeCount, TRAP_TYPE_COUNT * sizeof(UINT));
			for (FilteredEntity object : _entities.entities)
			{
				components::TrapComponent* p_obj_comp = object.getComponent<components::TrapComponent>();
				mObjectTypeCount[p_obj_comp->mObjectType - (GAME_OBJECT_TYPE_TRAP_OFFSET_TAG + 1)]++;
				mObjectTypeCount[p_obj_comp->mObjectType - (GAME_OBJECT_TYPE_TRAP_OFFSET_TAG + 1)]++;
			}

			// Set index to write to in RenderBuffer, per mesh
			UINT object_type_individual_index[TRAP_TYPE_COUNT] = { 0 };

			for (int i = 1; i < TRAP_TYPE_COUNT; i++)
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

				// Set "outline" mesh
				XMStoreFloat4x4(&mpBuffer[index].world, UtilityEcsFunctions::GetWorldMatrix(outline_transform));
				mpBuffer[index++].world._44 = PACK(0, 0, 0, 255);

				// Set trap mesh
				XMStoreFloat4x4(&mpBuffer[index].world, UtilityEcsFunctions::GetWorldMatrix(*p_transform_comp));
				mpBuffer[index++].world._44 = PACK(p_color_comp->red, p_color_comp->green, p_color_comp->blue, 255);
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
			for (int i = 0; i < TRAP_TYPE_COUNT; i++)
			{
				mObjectMeshRegion[i] = MeshContainer::GetMeshGPU(GAME_OBJECT_TYPE(i + (GAME_OBJECT_TYPE_TRAP_OFFSET_TAG + 1)));
			}

			mInstanceLayout.MeshCount = TRAP_TYPE_COUNT;
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

#pragma region PowerupLootRenderSystem
		PowerupLootRenderSystem::PowerupLootRenderSystem()
		{
			updateType = SystemUpdateType::MultiEntityUpdate;
			typeFilter.addRequirement(components::PowerupLootComponent::typeID);
			typeFilter.addRequirement(components::TransformComponent::typeID);
		}

		PowerupLootRenderSystem::~PowerupLootRenderSystem()
		{
			//
		}

		void PowerupLootRenderSystem::updateMultipleEntities(EntityIterator& _entities, float _delta)
		{
			/*
				We don't know the order of entities EntityIterator, meaning that we can't expect
				the entities to be ordered by mesh type like we want them to be in the RenderBuffer
				(output of this function).

				So, this function first calculate how many instances we have of each mesh. With this,
				we can calculate from which index in the RenderBuffer we can start writing each mesh
				to. Each mesh we care about in this function, that is tree, stone etc., has its own
				index counter.

				When rendering traps specific, we create two meshes for each trap. The first is the
				trap itself, and the other one is the same mesh but larger and black in color. This
				gives a simple mesh "outline" for viewing meshes during development.
			*/

			mObjectCount = _entities.entities.size();

			// Fetch pointer to write data to in RenderBuffer
			mpBuffer = (InputLayout*)mpRenderBuffer->GetBufferAddress(mObjectCount * systems::PowerupLootRenderSystem::GetPerInstanceSize());

			// Count how many instances we have per scene object mesh
			ZeroMemory(mObjectTypeCount, POWERUP_TYPE_COUNT * sizeof(UINT));
			for (FilteredEntity object : _entities.entities)
			{
				components::PowerupLootComponent* p_loot_comp = object.getComponent<components::PowerupLootComponent>();
				mObjectTypeCount[p_loot_comp->mPowerupType - (GAME_OBJECT_TYPE_POWERUP_OFFSET_TAG + 1)]++;
			}

			// Set index to write to in RenderBuffer, per mesh
			UINT object_type_individual_index[POWERUP_TYPE_COUNT] = { 0 };

			for (int i = 1; i < POWERUP_TYPE_COUNT; i++)
			{
				object_type_individual_index[i] = object_type_individual_index[i - 1] + mObjectTypeCount[i - 1];
			}

			// Iterate all objects and write their data to the RenderBuffer
			for (FilteredEntity powerup : _entities.entities)
			{
				components::PowerupLootComponent* p_loot_comp = powerup.getComponent<components::PowerupLootComponent>();
				components::TransformComponent* p_transform_comp = powerup.getComponent<components::TransformComponent>();

				// Get index, depending on mesh type
				UINT& index = object_type_individual_index[p_loot_comp->mPowerupType - (GAME_OBJECT_TYPE_POWERUP_OFFSET_TAG + 1)];

				/*
					Create a world matrix out of the powerup's transform.
					Place color in the last element of the world
					matrix. Color will be extracted in the shader.
				*/

				// Set "outline" mesh
				XMStoreFloat4x4(&mpBuffer[index].world, UtilityEcsFunctions::GetWorldMatrix(*p_transform_comp));
				mpBuffer[index++].world._44 = p_loot_comp->mColor;
			}

			mInstanceLayout.pInstanceCountPerMesh = mObjectTypeCount;
			mpRenderMgr->SetShaderModelLayout(mRenderProgram, mInstanceLayout);
		}

		void PowerupLootRenderSystem::Initialize(graphics::RenderManager* pRenderMgr, graphics::RenderBuffer* pRenderBuffer)
		{
			mpRenderMgr = pRenderMgr;

			/*
				Set up mesh region for all meshes that will be rendered.
			*/

			for (int i = 0; i < POWERUP_TYPE_COUNT; i++)
			{
				mObjectMeshRegion[i] = MeshContainer::GetMeshGPU(GAME_OBJECT_TYPE(i + (GAME_OBJECT_TYPE_POWERUP_OFFSET_TAG + 1)));
			}

			mInstanceLayout.MeshCount = POWERUP_TYPE_COUNT;
			mInstanceLayout.pMeshes = mObjectMeshRegion;
			mInstanceLayout.pInstanceCountPerMesh = &mObjectCount;

			const std::string vs = GetShaderFilepath("VS_Trap.cso");
			const std::string ps = GetShaderFilepath("PS_Ocean.cso");

			mRenderProgram = mpRenderMgr->CreateShaderProgram(
				vs.c_str(),
				ps.c_str(),
				systems::PowerupLootRenderSystem::GetPerInstanceSize());

			mpRenderBuffer = pRenderBuffer;
		}

		uint32_t PowerupLootRenderSystem::GetPerInstanceSize()
		{
			return sizeof(PowerupLootRenderSystem::InputLayout);
		}
#pragma endregion PowerupLootRenderSystem

#pragma region DefaultRenderSystem
		DefaultRenderSystem::DefaultRenderSystem()
		{
			updateType = SystemUpdateType::MultiEntityUpdate;
			typeFilter.addRequirement(components::DefaultRenderObjectComponent::typeID);
			typeFilter.addRequirement(components::TransformComponent::typeID);
			typeFilter.addRequirement(components::ColorComponent::typeID);

			mInstanceLayout = { 0 };
		}

		DefaultRenderSystem::~DefaultRenderSystem()
		{
			//
		}

		void DefaultRenderSystem::updateMultipleEntities(EntityIterator& _entities, float _delta)
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


			//// Count how many instances we have per scene object mesh
			ZeroMemory(mInstancePerMesh, GAME_OBJECT_TYPE_COUNT * sizeof(UINT));
			for (FilteredEntity object : _entities.entities)
			{
				components::DefaultRenderObjectComponent* p_obj_comp = object.getComponent<components::DefaultRenderObjectComponent>();

				mInstancePerMesh[p_obj_comp->type]++;
			}

			//// Fetch pointer to write data to in RenderBuffer
			mObjectCount = _entities.entities.size();
			//std::cout << mObjectCount << std::endl; //DEBUG PRINT
			mpBuffer = (InputLayout*)mpRenderBuffer->GetBufferAddress(mObjectCount * systems::DefaultRenderSystem::GetPerInstanceSize());


			// Set index to write to in RenderBuffer, per mesh
			UINT object_type_individual_index[GAME_OBJECT_TYPE_COUNT] = { 0 };

			for (int i = 1; i < GAME_OBJECT_TYPE_COUNT; i++)
			{
				object_type_individual_index[i] = object_type_individual_index[i - 1] + mInstancePerMesh[i - 1];
			}

			// Iterate all objects and write their data to the RenderBuffer
			for (FilteredEntity object : _entities.entities)
			{
				components::ColorComponent* p_color_comp = object.getComponent<components::ColorComponent>();
				components::TransformComponent* p_transform_comp = object.getComponent<components::TransformComponent>();
				components::DefaultRenderObjectComponent* p_object_comp = object.getComponent<components::DefaultRenderObjectComponent>();
				
				UINT& index = object_type_individual_index[p_object_comp->type];

				DirectX::XMMATRIX world = UtilityEcsFunctions::GetWorldMatrix(*p_transform_comp);

				XMStoreFloat4x4(&mpBuffer[index].world, world);
				mpBuffer[index].world._44 = PACK(p_color_comp->red, p_color_comp->green, p_color_comp->blue, 0);

				index++;
			}

			mInstanceLayout.MeshCount				= GAME_OBJECT_TYPE_COUNT;
			mInstanceLayout.pInstanceCountPerMesh	= mInstancePerMesh;

			mpRenderMgr->SetShaderModelLayout(mRenderProgram, mInstanceLayout);
		}

		void DefaultRenderSystem::Initialize(graphics::RenderManager* pRenderMgr, graphics::RenderBuffer* pRenderBuffer)
		{
			mpRenderMgr = pRenderMgr;

			for (UINT i = 0; i < GAME_OBJECT_TYPE_COUNT; i++)
			{
				mObjectMeshRegion[i] = MeshContainer::GetMeshGPU(i);
			}
			mInstanceLayout.pMeshes = mObjectMeshRegion;

			/*
				Add new meshes here

				Use 'mMap' to map scene object to scene mesh
				Use 'mColors' to apply color to each mesh (colors will not differ between same scene object)
			*/

			// End

			const std::string vs = GetShaderFilepath("VS_Weapon.cso");
			const std::string ps = GetShaderFilepath("PS_Default.cso");

			mRenderProgram = mpRenderMgr->CreateShaderProgram(
				vs.c_str(),
				ps.c_str(),
				systems::DefaultRenderSystem::GetPerInstanceSize());

			mpRenderBuffer = pRenderBuffer;
		}

		uint32_t DefaultRenderSystem::GetPerInstanceSize()
		{
			return sizeof(InputLayout);
		}
#pragma endregion DefaultRenderSystem
	}
}