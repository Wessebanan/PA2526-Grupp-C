#pragma once

#include "ecsSystemIncludes.h"
#include <DirectXMath.h>


namespace ecs
{
	namespace systems
	{
		class WorldSceneryUpdateSystem : public ECSSystem<WorldSceneryUpdateSystem>
		{
		public:
			WorldSceneryUpdateSystem();
			~WorldSceneryUpdateSystem();

			void Initialize();

			void updateEntity(FilteredEntity& _entityInfo, float _delta) override;
		private:
			DirectX::XMFLOAT3 mCenter;
			const float m_MAP_RADIOUS = 22.5f;
			const float m_WATER_HEIGHT = -1.2f;
		};
	
	}
}