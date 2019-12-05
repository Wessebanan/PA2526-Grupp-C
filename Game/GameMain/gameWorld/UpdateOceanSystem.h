#pragma once

#include <DirectXMath.h>
#include "ecsSystemIncludes.h"

namespace ecs
{
	namespace systems
	{
		class UpdateOceanSystem : public ECSSystem<UpdateOceanSystem>
		{
		public:

			UpdateOceanSystem();
			~UpdateOceanSystem();

			//void updateMultipleEntities(EntityIterator& rEntities, float delta) override;

			bool Initialize(float cycleDuration);
			
			void act(float _delta) override;

		private:

			DirectX::XMFLOAT3 mOceanCenter;
			float mTimeElapsed;
			float mCycleDuration;

			EntityIterator mOceanTiles;

			float* mWaveArray;
			float mFlatternOutFactor;
		};
	}
}