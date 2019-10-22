#pragma once

#include <DirectXMath.h>
#include "ecsSystemIncludes.h"
#include "../MeshContainer/MeshContainer.h"

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

			void SetBegin(void* pBufferStart);

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
		};

		class TileRenderSystem : public ECSSystem<TileRenderSystem>
		{
		public:

			TileRenderSystem();
			~TileRenderSystem();

			void updateMultipleEntities(EntityIterator& _entities, float _delta) override;

			void SetBegin(void* pBufferStart);

		private:

			struct InputLayout
			{
				float x, y, z;
				uint32_t color;
			};

			InputLayout* mpBuffer;
		};

		class SceneObjectRenderSystem : public ECSSystem<SceneObjectRenderSystem>
		{
		public:

			SceneObjectRenderSystem();
			~SceneObjectRenderSystem();

			void updateMultipleEntities(EntityIterator& _entities, float _delta) override;

			void SetBegin(void* pBufferStart);

		private:

			struct InputLayout
			{
				float x, y, z;
				uint32_t color;
			};

			InputLayout* mpBuffer;
		};
	}
}