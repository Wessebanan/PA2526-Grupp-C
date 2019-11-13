#include "ParticleECSSystems.h"
#include "ParticleECSComponents.h"

namespace ecs
{
	namespace systems
	{
#pragma region SmokeParticleRegion
		SmokeSpawnerSystem::SmokeSpawnerSystem()
		{
			updateType = EntityUpdate;
			typeFilter.addRequirement(components::ParticleSpawnerComponent::typeID);
			typeFilter.addRequirement(components::SmokeSpawnerComponent::typeID);
		}

		void SmokeSpawnerSystem::updateEntity(FilteredEntity& entity, float delta)
		{
			components::ParticleSpawnerComponent* p_spawner_component =
				entity.getComponent<components::ParticleSpawnerComponent>();

			components::SmokeSpawnerComponent* p_smoke_component =
				entity.getComponent<components::SmokeSpawnerComponent>();

			p_spawner_component->TimerSinceLastSpawn += delta;
			for (;
				p_spawner_component->TimerSinceLastSpawn > p_spawner_component->SpawnFrequency;
				p_spawner_component->TimerSinceLastSpawn -= p_spawner_component->SpawnFrequency)
			{
				components::ParticleComponent particle;
				components::SmokeParticleComponent smoke;

				particle.Position = p_spawner_component->StartPosition;
				smoke.TotalLifeDuration = p_spawner_component->LifeDuration;
				smoke.CurrentLifeDuration = smoke.TotalLifeDuration;

				particle.Red = 100 + rand() % 155;
				particle.Green = 100 + rand() % 155;
				particle.Blue = 100 + rand() % 155;
				particle.Scale = 40;
				smoke.MaxScale = particle.Scale;

				// Randomize x and y direction

				float x = ((float)rand() / (float)RAND_MAX) - 0.5f;
				float z = ((float)rand() / (float)RAND_MAX) - 0.5f;
				float speed = ((float)rand() / (float)RAND_MAX * 0.6f + 0.4f) * p_smoke_component->InitialVelocity;

				DirectX::XMVECTOR direction = DirectX::XMVectorSet(x, 1.0f, z, 0.0f);
				direction = DirectX::XMVector3Normalize(direction);
				direction = DirectX::XMVectorScale(direction, speed);
				DirectX::XMStoreFloat3(&smoke.Direction, direction);

				createEntity(particle, smoke);

				if (--p_smoke_component->SpawnCount <= 0)
				{
					removeEntity(entity.entity->getID());
					break;
				}
			}
		}

		SmokeUpdateSystem::SmokeUpdateSystem()
		{
			updateType = EntityUpdate;
			typeFilter.addRequirement(components::ParticleComponent::typeID);
			typeFilter.addRequirement(components::SmokeParticleComponent::typeID);
		}

		void SmokeUpdateSystem::updateEntity(FilteredEntity& entity, float delta)
		{
			const float gravity = 4.0f;

			components::ParticleComponent* p_particle_component =
				entity.getComponent<components::ParticleComponent>();

			components::SmokeParticleComponent* p_smoke_component =
				entity.getComponent<components::SmokeParticleComponent>();

			p_smoke_component->CurrentLifeDuration -= delta;
			if (p_smoke_component->CurrentLifeDuration <= 0.0f)
			{
				removeEntity(entity.entity->getID());
			}

			int scale = p_smoke_component->MaxScale * p_smoke_component->CurrentLifeDuration / p_smoke_component->TotalLifeDuration;
			p_particle_component->Scale = scale >= 0 ? scale : 0;

			DirectX::XMVECTOR position = DirectX::XMLoadFloat3(&p_particle_component->Position);
			DirectX::XMVECTOR direction = DirectX::XMLoadFloat3(&p_smoke_component->Direction);

			position = DirectX::XMVectorAdd(position, DirectX::XMVectorScale(direction, delta * p_smoke_component->CurrentLifeDuration / p_smoke_component->TotalLifeDuration));
			direction = DirectX::XMVectorAdd(direction, DirectX::XMVectorSet(0.0f, -gravity * delta, 0.0f, 0.0f));

			DirectX::XMStoreFloat3(&p_particle_component->Position, position);
			DirectX::XMStoreFloat3(&p_smoke_component->Direction, direction);
		}
#pragma endregion SmokeParticleRegion

#pragma region SplashParticleRegion
		SplashSpawnerSystem::SplashSpawnerSystem()
		{
			updateType = EntityUpdate;
			typeFilter.addRequirement(components::ParticleSpawnerComponent::typeID);
			typeFilter.addRequirement(components::SplashSpawnerComponent::typeID);
		}

		void SplashSpawnerSystem::updateEntity(FilteredEntity& entity, float delta)
		{
			components::ParticleSpawnerComponent* p_spawner_component =
				entity.getComponent<components::ParticleSpawnerComponent>();

			components::SplashSpawnerComponent* p_splash_component =
				entity.getComponent<components::SplashSpawnerComponent>();

			p_spawner_component->TimerSinceLastSpawn += delta;
			for (;
				p_spawner_component->TimerSinceLastSpawn > p_spawner_component->SpawnFrequency;
				p_spawner_component->TimerSinceLastSpawn -= p_spawner_component->SpawnFrequency)
			{
				components::ParticleComponent particle;
				components::SplashParticleComponent splash;

				particle.Position = p_spawner_component->StartPosition;
				splash.TotalLifeDuration = p_spawner_component->LifeDuration;
				splash.CurrentLifeDuration = splash.TotalLifeDuration;

				particle.Red = particle.Green = rand() % 155;
				//particle.Green = 100 + rand() % 155;
				particle.Blue = 200 + rand() % 55;
				particle.Scale = 40;
				splash.MaxScale = particle.Scale;

				// Randomize x and y direction

				float x = ((float)rand() / (float)RAND_MAX) - 0.5f;
				float z = ((float)rand() / (float)RAND_MAX) - 0.5f;
				float speed = ((float)rand() / (float)RAND_MAX * 0.6f + 0.4f) * p_splash_component->InitialVelocity;

				DirectX::XMVECTOR direction = DirectX::XMVectorSet(x, 1.0f, z, 0.0f);
				direction = DirectX::XMVector3Normalize(direction);
				direction = DirectX::XMVectorScale(direction, speed);
				DirectX::XMStoreFloat3(&splash.Direction, direction);

				createEntity(particle, splash);

				if (--p_splash_component->SpawnCount <= 0)
				{
					removeEntity(entity.entity->getID());
					break;
				}
			}
		}

		SplashUpdateSystem::SplashUpdateSystem()
		{
			updateType = EntityUpdate;
			typeFilter.addRequirement(components::ParticleComponent::typeID);
			typeFilter.addRequirement(components::SplashParticleComponent::typeID);
		}

		void SplashUpdateSystem::updateEntity(FilteredEntity& entity, float delta)
		{
			const float gravity = 10.0f;

			components::ParticleComponent* p_particle_component =
				entity.getComponent<components::ParticleComponent>();

			components::SplashParticleComponent* p_splash_component =
				entity.getComponent<components::SplashParticleComponent>();

			p_splash_component->CurrentLifeDuration -= delta;
			if (p_splash_component->CurrentLifeDuration <= 0.0f)
			{
				removeEntity(entity.entity->getID());
			}

			int scale = p_splash_component->MaxScale * p_splash_component->CurrentLifeDuration / p_splash_component->TotalLifeDuration;
			p_particle_component->Scale = scale >= 0 ? scale : 0;

			DirectX::XMVECTOR position = DirectX::XMLoadFloat3(&p_particle_component->Position);
			DirectX::XMVECTOR direction = DirectX::XMLoadFloat3(&p_splash_component->Direction);

			position = DirectX::XMVectorAdd(position, DirectX::XMVectorScale(direction, delta * p_splash_component->CurrentLifeDuration / p_splash_component->TotalLifeDuration));
			direction = DirectX::XMVectorAdd(direction, DirectX::XMVectorSet(0.0f, -gravity * delta, 0.0f, 0.0f));

			DirectX::XMStoreFloat3(&p_particle_component->Position, position);
			DirectX::XMStoreFloat3(&p_splash_component->Direction, direction);
		}
#pragma endregion SplashParticleRegion
	}
}