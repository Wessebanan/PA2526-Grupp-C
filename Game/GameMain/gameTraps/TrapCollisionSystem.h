#pragma once

#include <DirectXMath.h>
#include "ecsSystemIncludes.h"

namespace ecs
{
	namespace systems
	{
		/*
			-- TrapCollisionSystem
			Check collision between unit and traps. If a unit trigger a trap,
			this system creates an event for that trap type that corresponding
			trap trigger system will respond to.
		*/
		class TrapCollisionSystem : public ECSSystem<TrapCollisionSystem>
		{
		public:

			TrapCollisionSystem();
			~TrapCollisionSystem();

			void updateEntity(FilteredEntity& trap, float delta) override;

		private:

			const float TRIGGER_DISTANCE = 0.5f;
			const DirectX::XMFLOAT3 TRIGGER_POINT_OFFSET = { 0.f, 1.f, 0.f };
		};
	}
}