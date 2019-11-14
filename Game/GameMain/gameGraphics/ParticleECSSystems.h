#pragma once
#include "ecsSystemIncludes.h"
//#include "ParticleECSComponents.h"
namespace ecs
{
	namespace systems
	{
		class SmokeSpawnerSystem : public ECSSystem<SmokeSpawnerSystem>
		{
		public:
			SmokeSpawnerSystem();
			virtual ~SmokeSpawnerSystem() {}

			void updateEntity(FilteredEntity& entity, float delta) override;
		};

		class SmokeUpdateSystem : public ECSSystem<SmokeUpdateSystem>
		{
		public:
			SmokeUpdateSystem();
			virtual ~SmokeUpdateSystem() {}

			void updateEntity(FilteredEntity& entity, float delta) override;
		};

		class SplashSpawnerSystem : public ECSSystem<SplashSpawnerSystem>
		{
		public:
			SplashSpawnerSystem();
			virtual ~SplashSpawnerSystem() {}

			void updateEntity(FilteredEntity& entity, float delta) override;
		};

		class SplashUpdateSystem : public ECSSystem<SplashUpdateSystem>
		{
		public:
			SplashUpdateSystem();
			virtual ~SplashUpdateSystem() {}

			void updateEntity(FilteredEntity& entity, float delta) override;
		};
	}
}