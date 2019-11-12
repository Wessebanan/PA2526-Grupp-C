#pragma once

#include "ecs.h"
#include "GraphicsECSComponents.h"
#include "GraphicsECSSystems.h"

#define ECS_MAXIMUM_PARTICLES			(2000)
#define ECS_MAXIMUM_SMOKE_PARTICLES		(2000)

void InitParticles(ecs::EntityComponentSystem& rECS)
{
	rECS.reserveComponentCount<ecs::components::ParticleComponent>(ECS_MAXIMUM_PARTICLES);
	rECS.reserveComponentCount<ecs::components::SmokeParticleComponent>(ECS_MAXIMUM_SMOKE_PARTICLES);

	/*
		Create Particle Emitter Systems (Layer 7)
	*/
	rECS.createSystem<ecs::systems::SmokeSpawnerSystem>(7);

	/* 
		Create Particle Update Systems (Layer 8)
	*/
	rECS.createSystem<ecs::systems::SmokeUpdateSystem>(8);
}