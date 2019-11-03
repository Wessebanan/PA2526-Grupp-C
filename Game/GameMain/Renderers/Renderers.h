#pragma once

#include <DirectXMath.h>
#include "ecsSystemIncludes.h"
#include "../MeshContainer/MeshContainer.h"
#include "../../Graphics/includes/RenderManager.h"
#include "../gameGraphics/RenderBuffer.h"
#include "../gameSceneObjects/SceneObjectGlobals.h"
#include "../../Graphics/includes/StateManager.h"

//static const std::string GetShaderFilepath(const char* pFilename)
//{
//	std::string filepath = "..//";
//
//#ifdef _DEBUG
//	filepath.append("shaders_d//");
//#else
//	filepath.append("shaders//");
//#endif // _DEBUG
//	filepath.append(pFilename);
//
//	return filepath;
//}
//
//static inline uint32_t PACK(uint8_t c0, uint8_t c1, uint8_t c2, uint8_t c3) {
//	return (c0 << 24) | (c1 << 16) | (c2 << 8) | c3;
//}

namespace ecs
{
	namespace systems
	{
		/*
			-- Render Systems
			These ecs systems has the responsibility for rendering
			one or more meshes on the screen.

			Note that all these systems only write data to a
			RenderBuffer. The render calls happen in the game loop.

			Each render system fetch a pointer in the RenderBuffer
			for where they will write their data to. RenderBuffer
			is cleared every frame, so these pointers has to be
			fetched every update. This is because we want a dynamic
			reading of objects in the world. If we want an object
			to vanish, we just delete its entity from the ECS
			and the render systems never finds them.
			It.just.works.
		*/

		class UnitRenderSystem : public ECSSystem<UnitRenderSystem>
		{
		public:

			UnitRenderSystem();
			~UnitRenderSystem();

			void updateMultipleEntities(EntityIterator& _entities, float _delta) override;

			void Initialize(graphics::RenderManager* pRenderMgr, graphics::RenderBuffer* pRenderBuffer);

			static uint32_t GetPerInstanceSize();

		private:

			struct InputLayout
			{
				DirectX::XMFLOAT4X4 world;
				DirectX::XMFLOAT4X4 boneMatrices[63];
			};

			InputLayout* mpBuffer;
			ModelLoader::Skeleton* mpSkeleton;

			int mFrameCounter;
			int mAnimationFrameCounter;

			UINT mRenderProgram;
			graphics::RenderManager* mpRenderMgr;
			graphics::ShaderModelLayout mInstanceLayout;

			graphics::RenderBuffer* mpRenderBuffer;

			UINT mUnitCount;
			graphics::MeshRegion mUnitMeshRegion;
		};

		class TileRenderSystem : public ECSSystem<TileRenderSystem>
		{
		public:

			TileRenderSystem();
			~TileRenderSystem();

			void updateMultipleEntities(EntityIterator& _entities, float _delta) override;

			void Initialize(graphics::RenderManager* pRenderMgr, graphics::RenderBuffer* pRenderBuffer);

			static uint32_t GetPerInstanceSize();

		private:

			struct InputLayout
			{
				float x, y, z;
				uint32_t color;
			};

			InputLayout* mpBuffer;

			UINT mRenderProgram;
			graphics::RenderManager* mpRenderMgr;
			graphics::ShaderModelLayout mInstanceLayout;

			graphics::RenderBuffer* mpRenderBuffer;

			UINT mTileCount;
			graphics::MeshRegion mTileMeshRegion;
		};

		class OceanRenderSystem : public ECSSystem<OceanRenderSystem>
		{
		public:

			OceanRenderSystem();
			~OceanRenderSystem();

			void updateMultipleEntities(EntityIterator& _entities, float _delta) override;

			void Initialize(graphics::RenderManager* pRenderMgr, graphics::RenderBuffer* pRenderBuffer);

			static uint32_t GetPerInstanceSize();

		private:

			struct InputLayout
			{
				float x, y, z;
				uint32_t color;
			};

			InputLayout* mpBuffer;

			UINT mRenderProgram;
			graphics::RenderManager* mpRenderMgr;
			graphics::ShaderModelLayout mInstanceLayout;

			graphics::RenderBuffer* mpRenderBuffer;

			UINT mTileCount;
			graphics::MeshRegion mTileMeshRegion;
		};

		class WorldRenderSystem : public ECSSystem<WorldRenderSystem>
		{
		public:

			WorldRenderSystem();
			~WorldRenderSystem();

			void act(float _delta) override;
			void Initialize(
				graphics::RenderManager* pRenderMgr,
				graphics::StateManager* pStateMgr,
				void* pWorldMesh,
				UINT worldMeshVertexCount);

		private:

			struct VertexData
			{
				float x, y, z;
				float nx, ny, nz;
				uint32_t color;
			};

			UINT mInstanceCount;

			UINT mRenderProgram;
			UINT mPipelineState;
			graphics::RenderManager* mpRenderMgr;
			graphics::StateManager* mpStateMgr;
			graphics::ShaderModelLayout mInstanceLayout;
			graphics::MeshRegion mMeshRegion;

			void* mHeightData;
			UINT mHightDataSize;

			void* mpWorldVertices;
			UINT mWorldBufferSize;
		};

		class SceneObjectRenderSystem : public ECSSystem<SceneObjectRenderSystem>
		{
		public:

			SceneObjectRenderSystem();
			~SceneObjectRenderSystem();

			void updateMultipleEntities(EntityIterator& _entities, float _delta) override;

			void Initialize(graphics::RenderManager* pRenderMgr, graphics::RenderBuffer* pRenderBuffer);

			static uint32_t GetPerInstanceSize();

		private:

			struct InputLayout
			{
				float x, y, z;
				uint32_t color;
			};


			InputLayout* mpBuffer;

			UINT mRenderProgram;
			graphics::RenderManager* mpRenderMgr;
			graphics::ShaderModelLayout mInstanceLayout;

			graphics::RenderBuffer* mpRenderBuffer;

			UINT mObjectCount;
			graphics::MeshRegion mObjectMeshRegion[SCENE_OBJECT_COUNT];

			UINT mObjectTypeCount[SCENE_OBJECT_COUNT];
			UINT mMeshMap[SCENE_OBJECT_COUNT];
		};

		class SSAORenderSystem : public ECSSystem<SSAORenderSystem>
		{
		public:

			SSAORenderSystem();
			virtual ~SSAORenderSystem();

			void act(float _delta) override;

			void Initialize(
				graphics::MeshManager* pMeshMgr,
				const UINT clientWidth,
				const UINT clientHeight);

		private:

			/*struct InputLayout
			{
				float x, y, z;
				uint32_t color;
			};*/

			UINT mPipelineSSAO,
				mPipelineBlur,
				mPipelineCombine;

			UINT mShaderSSAO,
				mShaderBlur,
				mShaderBlur_v,
				mShaderCombine;

			graphics::RenderManager mRenderMgr;
			graphics::ShaderModelLayout mInstanceLayout;

			UINT mObjectCount;
			graphics::MeshRegion mScreenSpaceTriangle;
		};



		class WeaponRenderSystem : public ECSSystem<WeaponRenderSystem>
		{
		public:

			WeaponRenderSystem();
			virtual ~WeaponRenderSystem();

			void updateMultipleEntities(EntityIterator& _entities, float _delta) override;

			void Initialize(graphics::RenderManager* pRenderMgr, graphics::RenderBuffer* pRenderBuffer);

			static uint32_t GetPerInstanceSize();

		private:

			struct InputLayout
			{
				DirectX::XMFLOAT4X4 world;
			};

			InputLayout* mpBuffer;

			UINT mRenderProgram;
			graphics::RenderManager* mpRenderMgr;
			graphics::ShaderModelLayout mInstanceLayout;

			graphics::RenderBuffer* mpRenderBuffer;

			UINT mInstanceCount;
			graphics::MeshRegion mWeaponMeshRegion;
		};
	}
}