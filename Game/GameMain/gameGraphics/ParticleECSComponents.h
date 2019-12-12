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
			uint8_t Red, Green, Blue, Scale;

			DirectX::XMFLOAT3 Direction;
			float CurrentLifeDuration;
			float TotalLifeDuration;
			float Gravity = 1.0f;
			uint8_t MaxScale;
		};

		struct ParticleSpawnerComponent : public ECSComponent<ParticleSpawnerComponent>
		{
			DirectX::XMFLOAT3 StartPosition;
			float SpawnFrequency;
			float LifeDuration;

			float TimerSinceLastSpawn = 0.0f;
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
			uint8_t MaxScale;
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
			uint8_t MaxScale;
		};

		struct FireSpawnerComponent : public ECSComponent<FireSpawnerComponent>
		{
			float InitialVelocity;
			float SpawnCount;
		};

		struct FireParticleComponent : public ECSComponent<FireParticleComponent>
		{
			DirectX::XMFLOAT3 Direction;
			float CurrentLifeDuration;
			float TotalLifeDuration;
			uint8_t MaxScale;
		};

		struct BombSpawnerComponent : public ECSComponent<BombSpawnerComponent>
		{
			float InitialVelocity;
			float SpawnCount;
		};

		struct BombParticleComponent : public ECSComponent<BombParticleComponent>
		{
			DirectX::XMFLOAT3 Direction;
			float CurrentLifeDuration;
			float TotalLifeDuration;
			uint8_t MaxScale;

			uint8_t MaxRed;
			uint8_t MaxGreen;
			uint8_t MaxBlue;
		};

		struct TrapParticleSpawnComponent : public ECSComponent<TrapParticleSpawnComponent>
		{
			float InitialVelocity;
			float SpawnCount;
			unsigned char red, green, blue;
		};

		struct TrapParticleComponent : public ECSComponent<TrapParticleComponent>
		{
			float CurrentLifeDuration;
			float TotalLifeDuration;
			float Velocity;
		};
	}
}