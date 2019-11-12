#pragma once

#include "ecsComponentIncludes.h"
#include "../../Graphics/includes/RenderManager.h"
#include "../../Graphics/includes/MeshManager.h"
#include "../../Graphics/includes/StateManager.h"
#include "RenderBuffer.h"
#include "ShadowMapPipeline.h"
#include "ForwardRenderingPipeline.h"

namespace ecs
{
	namespace components
	{
		/*
			All components below are 'singleton-components'. There exist exactly one
			of each in the game, all attached to the same entity that we call the
			graphics entity. He's a cool dude.
		*/

		struct RenderManagerComponent : public ECSComponent<RenderManagerComponent>
		{
			graphics::RenderManager mgr;
		};

		struct MeshManagerComponent : public ECSComponent<MeshManagerComponent>
		{
			graphics::MeshManager mgr;
		};

		struct StateManagerComponent : public ECSComponent<StateManagerComponent>
		{
			graphics::StateManager mgr;
		};

		struct RenderBufferComponent : public ECSComponent<RenderBufferComponent>
		{
			graphics::RenderBuffer buffer;
			UINT bufferSize;
		};

		struct PipelineShadowMapComponent : public ECSComponent<PipelineShadowMapComponent>
		{
			graphics::SHADOW_MAP_PIPELINE_DESC pipelineDesc;
			graphics::SHADOW_MAP_PIPELINE_DATA data;
			UINT pipeline;
		};

		struct PipelineForwardComponent : public ECSComponent<PipelineForwardComponent>
		{
			graphics::FORWARD_RENDERING_PIPELINE_DESC pipelineDesc;
			graphics::FORWARD_RENDERING_PIPELINE_DATA data;
			UINT pipeline;
		};

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
	}
}