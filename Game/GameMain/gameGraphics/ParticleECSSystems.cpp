#include "ParticleECSSystems.h"
#include "ParticleECSComponents.h"

#include <algorithm>

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
				particle.Position				= p_spawner_component->StartPosition;
				particle.CurrentLifeDuration	= particle.TotalLifeDuration = p_spawner_component->LifeDuration;

				// Set Life Expectancy
				smoke.TotalLifeDuration		= p_spawner_component->LifeDuration;
				smoke.CurrentLifeDuration	= smoke.TotalLifeDuration;

				// Set Color
				particle.Red				= 100 + rand() % 155;
				particle.Green				= 100 + rand() % 155;
				particle.Blue				= 100 + rand() % 155;

				// Set Scale
				particle.Scale						= 40;
				particle.MaxScale = smoke.MaxScale = particle.Scale;

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
				DirectX::XMStoreFloat3(&particle.Direction, direction);

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
			/*typeFilter.addRequirement(components::ParticleComponent::typeID);
			typeFilter.addRequirement(components::SmokeParticleComponent::typeID);*/
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
				particle.CurrentLifeDuration = particle.TotalLifeDuration = p_spawner_component->LifeDuration;
				// Set Life Expectancy
				splash.TotalLifeDuration = p_spawner_component->LifeDuration;
				splash.CurrentLifeDuration = splash.TotalLifeDuration;

				// Set Color
				particle.Red = particle.Green = rand() % 155;
				particle.Blue = 200 + rand() % 55;
				particle.Scale = 40;
				particle.MaxScale = splash.MaxScale = particle.Scale;

				// Randomize x and y direction

				float x = ((float)rand() / (float)RAND_MAX) - 0.5f;
				float z = ((float)rand() / (float)RAND_MAX) - 0.5f;
				float speed = ((float)rand() / (float)RAND_MAX * 0.6f + 0.4f) * p_splash_component->InitialVelocity;

				// Normalize direction and scale with speed
				DirectX::XMVECTOR direction = DirectX::XMVectorSet(x, 1.0f, z, 0.0f);
				direction = DirectX::XMVector3Normalize(direction);
				direction = DirectX::XMVectorScale(direction, speed);
				
				// Store direction
				DirectX::XMStoreFloat3(&particle.Direction, direction);

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
		/*	typeFilter.addRequirement(components::ParticleComponent::typeID);
			typeFilter.addRequirement(components::SplashParticleComponent::typeID);*/
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
				particle.CurrentLifeDuration = particle.TotalLifeDuration = p_spawner_component->LifeDuration;
				// Set Life Expectancy
				splash.TotalLifeDuration = p_spawner_component->LifeDuration;
				splash.CurrentLifeDuration = splash.TotalLifeDuration;

				// Set Color
				particle.Red = 200 + rand() % 55;
				particle.Green = 100 + rand() % 55;
				particle.Blue = 60;
				particle.Scale = 60;
				particle.MaxScale = particle.Scale;

				// Randomize x and y direction

				float x = ((float)rand() / (float)RAND_MAX) - 0.5f;
				float z = ((float)rand() / (float)RAND_MAX) - 0.5f;
				float speed = ((float)rand() / (float)RAND_MAX * 0.6f + 0.4f) * p_splash_component->InitialVelocity;

				// Normalize direction and scale with speed
				DirectX::XMVECTOR direction = DirectX::XMVectorSet(x, 1.0f, z, 0.0f);
				direction = DirectX::XMVector3Normalize(direction);
				direction = DirectX::XMVectorScale(direction, speed);

				// Store direction
				DirectX::XMStoreFloat3(&particle.Direction, direction);

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
		/*	typeFilter.addRequirement(components::ParticleComponent::typeID);
			typeFilter.addRequirement(components::FireParticleComponent::typeID);*/
		}

		void FireUpdateSystem::updateEntity(FilteredEntity& entity, float delta)
		{
			const float gravity = 10.0f; // Gravity for particle

			components::ParticleComponent* p_particle_component =
				entity.getComponent<components::ParticleComponent>();

			components::FireParticleComponent* p_fire_component =
				entity.getComponent<components::FireParticleComponent>();

			// Decrease their life and terminate if life expectancy has reached
			p_fire_component->CurrentLifeDuration -= delta;
			if (p_fire_component->CurrentLifeDuration <= 0.0f)
			{
				removeEntity(entity.entity->getID());
			}

			// Decrease scale with proptional to how long they have lived [1;0]
			const int scale = p_fire_component->MaxScale * p_fire_component->CurrentLifeDuration / p_fire_component->TotalLifeDuration;
			p_particle_component->Scale = scale >= 0 ? scale : 0;

			// Get position and direction
			DirectX::XMVECTOR position = DirectX::XMLoadFloat3(&p_particle_component->Position);
			DirectX::XMVECTOR direction = DirectX::XMLoadFloat3(&p_fire_component->Direction);

			// Transform particle
			position = DirectX::XMVectorAdd(position, DirectX::XMVectorScale(direction, delta * p_fire_component->CurrentLifeDuration / p_fire_component->TotalLifeDuration));
			direction = DirectX::XMVectorAdd(direction, DirectX::XMVectorSet(0.0f, -gravity * delta, 0.0f, 0.0f));

			// Store position and direction
			DirectX::XMStoreFloat3(&p_particle_component->Position, position);
			DirectX::XMStoreFloat3(&p_fire_component->Direction, direction);
		}
#pragma endregion FireParticleRegion

#pragma region BombParticleRegion
		BombSpawnerSystem::BombSpawnerSystem()
		{
			updateType = EntityUpdate;
			typeFilter.addRequirement(components::ParticleSpawnerComponent::typeID);
			typeFilter.addRequirement(components::BombSpawnerComponent::typeID);
		}

		void BombSpawnerSystem::updateEntity(FilteredEntity& entity, float delta)
		{
			components::ParticleSpawnerComponent* p_spawner_component =
				entity.getComponent<components::ParticleSpawnerComponent>();

			components::BombSpawnerComponent* p_bomb_component =
				entity.getComponent<components::BombSpawnerComponent>();

			p_spawner_component->TimerSinceLastSpawn += delta;

			// As long as there are particles to spawn (lag can result in particles not being spawned)
			for (int i = 0; i < p_bomb_component->SpawnCount; i++)
			{
				/* Set Initial Values For Particle */
				components::ParticleComponent particle;
				components::BombParticleComponent bomb;

				// Set Position
				particle.Position = p_spawner_component->StartPosition;
				particle.CurrentLifeDuration = particle.TotalLifeDuration = p_spawner_component->LifeDuration;

				// Set Life Expectancy
				bomb.TotalLifeDuration = p_spawner_component->LifeDuration;
				bomb.CurrentLifeDuration = bomb.TotalLifeDuration;

				// Set Color
				bomb.MaxRed		= particle.Red		= 200;
				bomb.MaxGreen	= particle.Green	= 180;
				bomb.MaxBlue	= particle.Blue		= 0;

				// Set Scale
				particle.MaxScale = particle.Scale = 40;

				

				// Randomize x and y direction and speed
				const float x = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;
				const float y = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;
				const float z = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;
				const float speed = p_bomb_component->InitialVelocity;

				// Normalize direction and scale with speed
				DirectX::XMVECTOR direction = DirectX::XMVectorSet(x, y, z, 0.0f);
				direction = DirectX::XMVector3Normalize(direction);
				direction = DirectX::XMVectorScale(direction, speed);

				// Store direction
				DirectX::XMStoreFloat3(&particle.Direction, direction);

				// Create Particle
				createEntity(particle, bomb);
			}

			removeEntity(entity.entity->getID());
		}

		BombUpdateSystem::BombUpdateSystem()
		{
			updateType = EntityUpdate;
		/*	typeFilter.addRequirement(components::ParticleComponent::typeID);
			typeFilter.addRequirement(components::BombParticleComponent::typeID);*/
		}

		void BombUpdateSystem::updateEntity(FilteredEntity& entity, float delta)
		{
			components::ParticleComponent* p_particle_component =
				entity.getComponent<components::ParticleComponent>();

			components::BombParticleComponent* p_bomb_component =
				entity.getComponent<components::BombParticleComponent>();

			// Decrease their life and terminate if life expectancy has reached
			p_bomb_component->CurrentLifeDuration -= delta;
			if (p_bomb_component->CurrentLifeDuration <= 0.0f)
			{
				removeEntity(entity.entity->getID());
			}

			// Life Left in Percentage
			const float life_left_p = p_bomb_component->CurrentLifeDuration / p_bomb_component->TotalLifeDuration;

			// Decrease scale with proptional to how long they have lived [1;0]
			p_particle_component->Scale = p_bomb_component->MaxScale * life_left_p;

			const float temp = life_left_p - 0.35f;
			const float life_left_p_half = temp > 0.0f ? temp : 0.0f;

			p_particle_component->Red	= (float)p_bomb_component->MaxRed * life_left_p;
			p_particle_component->Green = (float)p_bomb_component->MaxGreen * life_left_p_half;

			// Get position and direction
			DirectX::XMVECTOR position = DirectX::XMLoadFloat3(&p_particle_component->Position);
			DirectX::XMVECTOR direction = DirectX::XMLoadFloat3(&p_bomb_component->Direction);

			// Transform particle
			position = DirectX::XMVectorAdd(position, DirectX::XMVectorScale(direction, delta));

			// Store position and direction
			DirectX::XMStoreFloat3(&p_particle_component->Position, position);
			DirectX::XMStoreFloat3(&p_bomb_component->Direction, direction);
		}
#pragma endregion BombParticleRegion

#pragma region ParticleTrapSpawnerSystem
		ParticleTrapSpawnerSystem::ParticleTrapSpawnerSystem()
		{
			updateType = EntityUpdate;
			typeFilter.addRequirement(components::ParticleSpawnerComponent::typeID);
			typeFilter.addRequirement(components::TrapParticleSpawnComponent::typeID);
		}

		void ParticleTrapSpawnerSystem::updateEntity(FilteredEntity& entity, float delta)
		{
			components::ParticleSpawnerComponent* p_spawner_component =
				entity.getComponent<components::ParticleSpawnerComponent>();

			components::TrapParticleSpawnComponent* p_trap_spawn_component =
				entity.getComponent<components::TrapParticleSpawnComponent>();

			p_spawner_component->TimerSinceLastSpawn += delta;

			// As long as there are particles to spawn (lag can result in particles not being spawned)
			for (;
				p_spawner_component->TimerSinceLastSpawn > p_spawner_component->SpawnFrequency;
				p_spawner_component->TimerSinceLastSpawn -= p_spawner_component->SpawnFrequency)
			{
				/* Set Initial Values For Particle */
				components::ParticleComponent particle;
				components::TrapParticleComponent trap_particle;

				DirectX::XMVECTOR start_pos;
				DirectX::XMLoadFloat3(&p_spawner_component->StartPosition);				
				DirectX::XMFLOAT3 offset;
				
				DirectX::XMStoreFloat3(&offset,
					DirectX::XMVector3Transform(
						{ 0.8f, 0.f, 0.f, 0.f },
						DirectX::XMMatrixRotationAxis(
							{ 0.f, 1.f, 0.f, 0.f },
							(float)(rand() % 360) * (3.14159f / 180.f))));

				// Set Position
				particle.CurrentLifeDuration = particle.TotalLifeDuration = p_spawner_component->LifeDuration;
				particle.Position = p_spawner_component->StartPosition;
				particle.Position.x += offset.x;
				particle.Position.y += offset.y;
				particle.Position.z += offset.z;

				//particle.Position = p_spawner_component->StartPosition;

				// Set Life Expectancy
				trap_particle.TotalLifeDuration = p_spawner_component->LifeDuration;
				trap_particle.CurrentLifeDuration = trap_particle.TotalLifeDuration;

				// Set Color
				particle.Red = p_trap_spawn_component->red;
				particle.Green = p_trap_spawn_component->green;
				particle.Blue = p_trap_spawn_component->blue;

				// Set Scale
				particle.Scale = 40;

				// Randomize x and y direction and speed
				float x = ((float)rand() / (float)RAND_MAX) - 0.5f;
				float z = ((float)rand() / (float)RAND_MAX) - 0.5f;
				trap_particle.Velocity = ((float)rand() / (float)RAND_MAX * 0.6f + 0.4f) * p_trap_spawn_component->InitialVelocity;

				particle.Direction = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);

				// Create Particle
				createEntity(particle, trap_particle);

				// Remove spawner component if they have spawned the specified amount
				if (--p_trap_spawn_component->SpawnCount <= 0)
				{
					removeEntity(entity.entity->getID());
					break;
				}
			}
		}
#pragma endregion ParticleTrapSpawnerSystem

#pragma region ParticleTrapUpdateSystem
		ParticleTrapUpdateSystem::ParticleTrapUpdateSystem()
		{
			updateType = EntityUpdate;
		/*	typeFilter.addRequirement(components::ParticleComponent::typeID);
			typeFilter.addRequirement(components::TrapParticleComponent::typeID);*/
		}

		void ParticleTrapUpdateSystem::updateEntity(FilteredEntity& entity, float delta)
		{
			const float gravity = 4.0f; // Gravity for particle

			components::ParticleComponent* p_particle_component =
				entity.getComponent<components::ParticleComponent>();

			components::TrapParticleComponent* p_trap_component =
				entity.getComponent<components::TrapParticleComponent>();

			// Decrease their life and terminate if life expectancy has reached
			p_trap_component->CurrentLifeDuration -= delta;
			if (p_trap_component->CurrentLifeDuration <= 0.0f)
			{
				removeEntity(entity.entity->getID());
			}

			// Decrease scale with proptional to how long they have lived [1;0]
			const int scale = 30.f * p_trap_component->CurrentLifeDuration / p_trap_component->TotalLifeDuration;
			p_particle_component->Scale = scale >= 0 ? scale : 0;

			const DirectX::XMFLOAT3 UP_DIRECTION = { 0.f, 1.f * p_trap_component->Velocity, 0.f };

			// Get position and direction
			DirectX::XMVECTOR position = DirectX::XMLoadFloat3(&p_particle_component->Position);
			DirectX::XMVECTOR direction = DirectX::XMLoadFloat3(&UP_DIRECTION);

			// Transform particle
			position = DirectX::XMVectorAdd(position, DirectX::XMVectorScale(direction, delta * p_trap_component->CurrentLifeDuration / p_trap_component->TotalLifeDuration));
			direction = DirectX::XMVectorAdd(direction, DirectX::XMVectorSet(0.0f, -gravity * delta, 0.0f, 0.0f));

			// Store position and direction
			DirectX::XMStoreFloat3(&p_particle_component->Position, position);
			p_trap_component->Velocity -= gravity * delta;
		}
#pragma endregion ParticleTrapUpdateSystem

		DefaultParticleUpdateSystem::DefaultParticleUpdateSystem()
		{
			updateType = EntityUpdate;
			typeFilter.addRequirement(components::ParticleComponent::typeID);
		}

		void DefaultParticleUpdateSystem::updateEntity(FilteredEntity& entity, float delta)
		{
			components::ParticleComponent* p_particle_component =
				entity.getComponent<components::ParticleComponent>();

			// Decrease their life and terminate if life expectancy has reached
			p_particle_component->CurrentLifeDuration -= delta;
			if (p_particle_component->CurrentLifeDuration <= 0.0f)
			{
				removeEntity(entity.entity->getID());
			}

			// Life Left in Percentage
			const float life_left_p = p_particle_component->CurrentLifeDuration / p_particle_component->TotalLifeDuration;

			// Decrease scale with proptional to how long they have lived [1;0]
			p_particle_component->Scale = p_particle_component->MaxScale * life_left_p;

			// Get position and direction
			DirectX::XMVECTOR position	= DirectX::XMLoadFloat3(&p_particle_component->Position);
			DirectX::XMVECTOR direction = DirectX::XMLoadFloat3(&p_particle_component->Direction);

			// Transform particle
			position = DirectX::XMVectorAdd(position, DirectX::XMVectorScale(direction, delta * p_particle_component->CurrentLifeDuration / p_particle_component->TotalLifeDuration));
			direction = DirectX::XMVectorAdd(direction, DirectX::XMVectorSet(0.0f, -p_particle_component->Gravity * delta, 0.0f, 0.0f));

			// Store position and direction
			DirectX::XMStoreFloat3(&p_particle_component->Position, position);
			DirectX::XMStoreFloat3(&p_particle_component->Direction, direction);
		}
	}
}