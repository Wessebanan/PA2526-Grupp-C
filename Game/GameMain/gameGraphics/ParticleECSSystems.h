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

		class FireSpawnerSystem : public ECSSystem<FireSpawnerSystem>
		{
		public:
			FireSpawnerSystem();
			virtual ~FireSpawnerSystem() {}

			void updateEntity(FilteredEntity& entity, float delta) override;
		};

		class FireUpdateSystem : public ECSSystem<FireUpdateSystem>
		{
		public:
			FireUpdateSystem();
			virtual ~FireUpdateSystem() {}

			void updateEntity(FilteredEntity& entity, float delta) override;
		};

		class BombSpawnerSystem : public ECSSystem<BombSpawnerSystem>
		{
		public:
			BombSpawnerSystem();
			virtual ~BombSpawnerSystem() {}

			void updateEntity(FilteredEntity& entity, float delta) override;
		};

		class BombUpdateSystem : public ECSSystem<BombUpdateSystem>
		{
		public:
			BombUpdateSystem();
			virtual ~BombUpdateSystem() {}

			void updateEntity(FilteredEntity& entity, float delta) override;
		};

		class ParticleTrapSpawnerSystem : public ECSSystem<ParticleTrapSpawnerSystem>
		{
		public:
			ParticleTrapSpawnerSystem();
			virtual ~ParticleTrapSpawnerSystem() {}

			void updateEntity(FilteredEntity& entity, float delta) override;
		};

		class ParticleTrapUpdateSystem : public ECSSystem<ParticleTrapUpdateSystem>
		{
		public:
			ParticleTrapUpdateSystem();
			virtual ~ParticleTrapUpdateSystem() {}

			void updateEntity(FilteredEntity& entity, float delta) override;
		};
	}
}