#pragma once

#include <DirectXMath.h>
#include "ecsSystemIncludes.h"
#include "../MeshContainer/MeshContainer.h"
#include "../../Graphics/includes/RenderManager.h"

namespace ecs
{
	namespace systems
	{
		class UnitRenderSystem : public ECSSystem<UnitRenderSystem>
		{
		public:

			UnitRenderSystem();
			~UnitRenderSystem();

			void updateMultipleEntities(EntityIterator& _entities, float _delta) override;

			void Initialize(graphics::RenderManager* pRenderMgr);

			void SetBegin(void* pBufferStart);

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

			UINT mUnitCount;
			graphics::MeshRegion mUnitMeshRegion;
		};

		class TileRenderSystem : public ECSSystem<TileRenderSystem>
		{
		public:

			TileRenderSystem();
			~TileRenderSystem();

			void updateMultipleEntities(EntityIterator& _entities, float _delta) override;

			void Initialize(graphics::RenderManager* pRenderMgr);

			void SetBegin(void* pBufferStart);

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

			UINT mTileCount;
			graphics::MeshRegion mTileMeshRegion;
		};

		class SceneObjectRenderSystem : public ECSSystem<SceneObjectRenderSystem>
		{
		public:

			SceneObjectRenderSystem();
			~SceneObjectRenderSystem();

			void updateMultipleEntities(EntityIterator& _entities, float _delta) override;

			void Initialize(graphics::RenderManager* pRenderMgr);

			void SetBegin(void* pBufferStart);

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

			UINT mObjectCount;
			graphics::MeshRegion mObjectMeshRegion;
		};
	}
}