#pragma once
#include <DirectXMath.h>
#include "ecsComponentIncludes.h"

namespace ecs
{
	namespace components
	{
		// --- Default Particle Data ---
		struct ParticleComponent : public ECSComponent<ParticleComponent>
		{
			DirectX::XMFLOAT3 Position;
			char Red, Green, Blue, Scale;
		};

		struct ParticleSpawnerComponent : public ECSComponent<ParticleSpawnerComponent>
		{
			DirectX::XMFLOAT3 StartPosition;
			float SpawnFrequency;
			float LifeDuration;

			float TimerSinceLastSpawn;
		};

		// --- Specific Particle Data ---
		struct SmokeSpawnerComponent : public ECSComponent<SmokeSpawnerComponent>
		{
			float InitialVelocity;
			float SpawnCount;
		};

		struct SmokeParticleComponent : public ECSComponent<SmokeParticleComponent>
		{
			DirectX::XMFLOAT3 Direction;
			float CurrentLifeDuration;
			float TotalLifeDuration;
			char MaxScale;
		};

		struct SplashSpawnerComponent : public ECSComponent<SplashSpawnerComponent>
		{
			float InitialVelocity;
			float SpawnCount;
		};

		struct SplashParticleComponent : public ECSComponent<SplashParticleComponent>
		{
			DirectX::XMFLOAT3 Direction;
			float CurrentLifeDuration;
			float TotalLifeDuration;
			char MaxScale;
		};
	}
}