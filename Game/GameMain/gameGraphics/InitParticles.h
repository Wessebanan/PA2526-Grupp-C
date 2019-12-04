#pragma once

#include "ecs.h"
#include "ParticleECSComponents.h"
#include "ParticleECSSystems.h"

/*
	!!!	IMPORTANT : READ BEFORE ADDING PARTICLE SYSTEM !!!

	Make sure 'ECS_MAXIMUM_PARTICLE_SYSTEMS_PER_TYPE' has the correct value

	Add new particle type to the list below
*/
constexpr uint32_t ECS_MAXIMUM_PARTICLE_SYSTEMS_PER_TYPE = 4; // Change this if adding or removing system

constexpr uint32_t ECS_MAXIMUM_PARTICLES_PER_TYPE = 4000;
constexpr uint32_t ECS_MAXIMUM_PARTICLES = ECS_MAXIMUM_PARTICLES_PER_TYPE * ECS_MAXIMUM_PARTICLE_SYSTEMS_PER_TYPE;


template<class SpawnerSystem, class UpdateSystem, class ParticleComponent>
void CreateParticleType(ecs::EntityComponentSystem& rECS)
{
	rECS.reserveComponentCount<ParticleComponent>(ECS_MAXIMUM_PARTICLES_PER_TYPE);
	rECS.createSystem<SpawnerSystem>(7);
	rECS.createSystem<UpdateSystem>(8);
}

void InitParticles(ecs::EntityComponentSystem& rECS)
{
	// !! DO NOT TOUCH !!
	rECS.reserveComponentCount<ecs::components::ParticleComponent>(ECS_MAXIMUM_PARTICLES);
	
	/* 
		THIS IS THE !! LIST !! 

		Here you can simply just add a new line for the desired particle system

		! REMINDER ! 
		Create your own:
			- Spawner System
			- Update System
			- Type Specific Particle and Spawner (Components)

		Files can be found in Graphics -> ECS

			'ParticleECSComponents.h'
			'ParticleECSSystems.h'
			'ParticleECSSystems.cpp'
	*/

	CreateParticleType<SmokeSpawnerSystem, SmokeUpdateSystem, SmokeParticleComponent>(rECS);
	CreateParticleType<SplashSpawnerSystem, SplashUpdateSystem, SplashParticleComponent>(rECS);
	CreateParticleType<FireSpawnerSystem, FireUpdateSystem, FireParticleComponent>(rECS);
	CreateParticleType<BombSpawnerSystem, BombUpdateSystem, BombParticleComponent>(rECS);
	CreateParticleType<ParticleTrapSpawnerSystem, ParticleTrapUpdateSystem, TrapParticleComponent>(rECS);
}