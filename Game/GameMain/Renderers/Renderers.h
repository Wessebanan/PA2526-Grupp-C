#pragma once

#include <DirectXMath.h>
#include "ecsSystemIncludes.h"
#include "../MeshContainer/MeshContainer.h"
#include "../../Graphics/includes/RenderManager.h"
#include "../gameGraphics/RenderBuffer.h"

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
			graphics::MeshRegion mObjectMeshRegion;
		};
	}
}