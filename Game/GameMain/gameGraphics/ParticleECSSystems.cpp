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

			// As long as there are particles to spawn (lag can result in particles not being spawned)
			for (;
				p_spawner_component->TimerSinceLastSpawn > p_spawner_component->SpawnFrequency;
				p_spawner_component->TimerSinceLastSpawn -= p_spawner_component->SpawnFrequency)
			{
				/* Set Initial Values For Particle */
				components::ParticleComponent particle;
				components::SmokeParticleComponent smoke;

				// Set Position
				particle.Position			= p_spawner_component->StartPosition;

				// Set Life Expectancy
				smoke.TotalLifeDuration		= p_spawner_component->LifeDuration;
				smoke.CurrentLifeDuration	= smoke.TotalLifeDuration;

				// Set Color
				particle.Red				= 100 + rand() % 155;
				particle.Green				= 100 + rand() % 155;
				particle.Blue				= 100 + rand() % 155;

				// Set Scale
				particle.Scale				= 40;
				smoke.MaxScale				= particle.Scale;

				// Randomize x and y direction and speed
				float x = ((float)rand() / (float)RAND_MAX) - 0.5f;
				float z = ((float)rand() / (float)RAND_MAX) - 0.5f;
				float speed = ((float)rand() / (float)RAND_MAX * 0.6f + 0.4f) * p_smoke_component->InitialVelocity;

				// Normalize direction and scale with speed
				DirectX::XMVECTOR direction = DirectX::XMVectorSet(x, 1.0f, z, 0.0f);
				direction = DirectX::XMVector3Normalize(direction);
				direction = DirectX::XMVectorScale(direction, speed);

				// Store direction
				DirectX::XMStoreFloat3(&smoke.Direction, direction);

				// Create Particle
				createEntity(particle, smoke);

				// Remove spawner component if they have spawned the specified amount
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
			const float gravity = 4.0f; // Gravity for particle

			components::ParticleComponent* p_particle_component =
				entity.getComponent<components::ParticleComponent>();

			components::SmokeParticleComponent* p_smoke_component =
				entity.getComponent<components::SmokeParticleComponent>();

			// Decrease their life and terminate if life expectancy has reached
			p_smoke_component->CurrentLifeDuration -= delta;
			if (p_smoke_component->CurrentLifeDuration <= 0.0f)
			{
				removeEntity(entity.entity->getID());
			}

			// Decrease scale with proptional to how long they have lived [1;0]
			const int scale = p_smoke_component->MaxScale * p_smoke_component->CurrentLifeDuration / p_smoke_component->TotalLifeDuration;
			p_particle_component->Scale = scale >= 0 ? scale : 0;

			// Get position and direction
			DirectX::XMVECTOR position = DirectX::XMLoadFloat3(&p_particle_component->Position);
			DirectX::XMVECTOR direction = DirectX::XMLoadFloat3(&p_smoke_component->Direction);

			// Transform particle
			position = DirectX::XMVectorAdd(position, DirectX::XMVectorScale(direction, delta * p_smoke_component->CurrentLifeDuration / p_smoke_component->TotalLifeDuration));
			direction = DirectX::XMVectorAdd(direction, DirectX::XMVectorSet(0.0f, -gravity * delta, 0.0f, 0.0f));

			// Store position and direction
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
			// As long as there are particles to spawn (lag can result in particles not being spawned)
			for (;
				p_spawner_component->TimerSinceLastSpawn > p_spawner_component->SpawnFrequency;
				p_spawner_component->TimerSinceLastSpawn -= p_spawner_component->SpawnFrequency)
			{
				/* Set Initial Values For Particle */
				components::ParticleComponent particle;
				components::SplashParticleComponent splash;

				// Set Position
				particle.Position = p_spawner_component->StartPosition;
				// Set Life Expectancy
				splash.TotalLifeDuration = p_spawner_component->LifeDuration;
				splash.CurrentLifeDuration = splash.TotalLifeDuration;

				// Set Color
				particle.Red = particle.Green = rand() % 155;
				particle.Blue = 200 + rand() % 55;
				particle.Scale = 40;
				splash.MaxScale = particle.Scale;

				// Randomize x and y direction

				float x = ((float)rand() / (float)RAND_MAX) - 0.5f;
				float z = ((float)rand() / (float)RAND_MAX) - 0.5f;
				float speed = ((float)rand() / (float)RAND_MAX * 0.6f + 0.4f) * p_splash_component->InitialVelocity;

				// Normalize direction and scale with speed
				DirectX::XMVECTOR direction = DirectX::XMVectorSet(x, 1.0f, z, 0.0f);
				direction = DirectX::XMVector3Normalize(direction);
				direction = DirectX::XMVectorScale(direction, speed);
				
				// Store direction
				DirectX::XMStoreFloat3(&splash.Direction, direction);

				// Create Particle
				createEntity(particle, splash);

				// Remove spawner component if they have spawned the specified amount
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
			const float gravity = 10.0f; // Gravity for particle

			components::ParticleComponent* p_particle_component =
				entity.getComponent<components::ParticleComponent>();

			components::SplashParticleComponent* p_splash_component =
				entity.getComponent<components::SplashParticleComponent>();

			// Decrease their life and terminate if life expectancy has reached
			p_splash_component->CurrentLifeDuration -= delta;
			if (p_splash_component->CurrentLifeDuration <= 0.0f)
			{
				removeEntity(entity.entity->getID());
			}

			// Decrease scale with proptional to how long they have lived [1;0]
			const int scale = p_splash_component->MaxScale * p_splash_component->CurrentLifeDuration / p_splash_component->TotalLifeDuration;
			p_particle_component->Scale = scale >= 0 ? scale : 0;

			// Get position and direction
			DirectX::XMVECTOR position = DirectX::XMLoadFloat3(&p_particle_component->Position);
			DirectX::XMVECTOR direction = DirectX::XMLoadFloat3(&p_splash_component->Direction);

			// Transform particle
			position = DirectX::XMVectorAdd(position, DirectX::XMVectorScale(direction, delta * p_splash_component->CurrentLifeDuration / p_splash_component->TotalLifeDuration));
			direction = DirectX::XMVectorAdd(direction, DirectX::XMVectorSet(0.0f, -gravity * delta, 0.0f, 0.0f));

			// Store position and direction
			DirectX::XMStoreFloat3(&p_particle_component->Position, position);
			DirectX::XMStoreFloat3(&p_splash_component->Direction, direction);
		}
#pragma endregion SplashParticleRegion

#pragma region FireParticleRegion
		FireSpawnerSystem::FireSpawnerSystem()
		{
			updateType = EntityUpdate;
			typeFilter.addRequirement(components::ParticleSpawnerComponent::typeID);
			typeFilter.addRequirement(components::FireSpawnerComponent::typeID);
		}

		void FireSpawnerSystem::updateEntity(FilteredEntity& entity, float delta)
		{
			components::ParticleSpawnerComponent* p_spawner_component =
				entity.getComponent<components::ParticleSpawnerComponent>();

			components::FireSpawnerComponent* p_splash_component =
				entity.getComponent<components::FireSpawnerComponent>();

			p_spawner_component->TimerSinceLastSpawn += delta;
			// As long as there are particles to spawn (lag can result in particles not being spawned)
			for (;
				p_spawner_component->TimerSinceLastSpawn > p_spawner_component->SpawnFrequency;
				p_spawner_component->TimerSinceLastSpawn -= p_spawner_component->SpawnFrequency)
			{
				/* Set Initial Values For Particle */
				components::ParticleComponent particle;
				components::FireParticleComponent splash;

				// Set Position
				particle.Position = p_spawner_component->StartPosition;
				// Set Life Expectancy
				splash.TotalLifeDuration = p_spawner_component->LifeDuration;
				splash.CurrentLifeDuration = splash.TotalLifeDuration;

				// Set Color
				particle.Red = 200 + rand() % 55;
				particle.Green = 100 + rand() % 55;
				particle.Blue = 60;
				particle.Scale = 60;
				splash.MaxScale = particle.Scale;

				// Randomize x and y direction

				float x = ((float)rand() / (float)RAND_MAX) - 0.5f;
				float z = ((float)rand() / (float)RAND_MAX) - 0.5f;
				float speed = ((float)rand() / (float)RAND_MAX * 0.6f + 0.4f) * p_splash_component->InitialVelocity;

				// Normalize direction and scale with speed
				DirectX::XMVECTOR direction = DirectX::XMVectorSet(x, 1.0f, z, 0.0f);
				direction = DirectX::XMVector3Normalize(direction);
				direction = DirectX::XMVectorScale(direction, speed);

				// Store direction
				DirectX::XMStoreFloat3(&splash.Direction, direction);

				// Create Particle
				createEntity(particle, splash);

				// Remove spawner component if they have spawned the specified amount
				if (--p_splash_component->SpawnCount <= 0)
				{
					removeEntity(entity.entity->getID());
					break;
				}
			}
		}

		FireUpdateSystem::FireUpdateSystem()
		{
			updateType = EntityUpdate;
			typeFilter.addRequirement(components::ParticleComponent::typeID);
			typeFilter.addRequirement(components::FireParticleComponent::typeID);
		}

		void FireUpdateSystem::updateEntity(FilteredEntity& entity, float delta)
		{
			const float gravity = 10.0f; // Gravity for particle

			components::ParticleComponent* p_particle_component =
				entity.getComponent<components::ParticleComponent>();

			components::FireParticleComponent* p_splash_component =
				entity.getComponent<components::FireParticleComponent>();

			// Decrease their life and terminate if life expectancy has reached
			p_splash_component->CurrentLifeDuration -= delta;
			if (p_splash_component->CurrentLifeDuration <= 0.0f)
			{
				removeEntity(entity.entity->getID());
			}

			// Decrease scale with proptional to how long they have lived [1;0]
			const int scale = p_splash_component->MaxScale * p_splash_component->CurrentLifeDuration / p_splash_component->TotalLifeDuration;
			p_particle_component->Scale = scale >= 0 ? scale : 0;

			// Get position and direction
			DirectX::XMVECTOR position = DirectX::XMLoadFloat3(&p_particle_component->Position);
			DirectX::XMVECTOR direction = DirectX::XMLoadFloat3(&p_splash_component->Direction);

			// Transform particle
			position = DirectX::XMVectorAdd(position, DirectX::XMVectorScale(direction, delta * p_splash_component->CurrentLifeDuration / p_splash_component->TotalLifeDuration));
			direction = DirectX::XMVectorAdd(direction, DirectX::XMVectorSet(0.0f, -gravity * delta, 0.0f, 0.0f));

			// Store position and direction
			DirectX::XMStoreFloat3(&p_particle_component->Position, position);
			DirectX::XMStoreFloat3(&p_splash_component->Direction, direction);
		}
#pragma endregion SplashParticleRegion
}
}