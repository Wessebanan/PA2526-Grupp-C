#include "GraphicsECSSystems.h"
#include "../gameUtility/CameraComponents.h"
#include "../Renderers/Renderers.h"

namespace ecs
{
	namespace systems
	{
		/*
			Help function
		*/
		void SetViewMatrix(
			DirectX::XMFLOAT4X4& rViewMatrix,
			const float x,
			const float y,
			const float z,
			const float dx,
			const float dy,
			const float dz)
		{
			DirectX::XMStoreFloat4x4(&rViewMatrix,
				DirectX::XMMatrixLookToLH(
					{ x, y, z },
					{ dx, dy,  dz },
					{ 0.0f, 1.0f,  0.0f }
			));
		}



		RenderBufferResetSystem::RenderBufferResetSystem()
		{
			updateType = EntityUpdate;
			typeFilter.addRequirement(components::RenderBufferComponent::typeID);
		}

		void RenderBufferResetSystem::updateEntity(FilteredEntity& entity, float delta)
		{
			components::RenderBufferComponent* p_buffer = entity.getComponent<components::RenderBufferComponent>();
			p_buffer->buffer.Reset();
		}





		UploadRenderBufferSystem::UploadRenderBufferSystem()
		{
			updateType = EntityUpdate;
			typeFilter.addRequirement(components::RenderManagerComponent::typeID);
			typeFilter.addRequirement(components::RenderBufferComponent::typeID);
		}

		void UploadRenderBufferSystem::updateEntity(FilteredEntity& entity, float delta)
		{
			components::RenderManagerComponent* p_mgr = entity.getComponent<components::RenderManagerComponent>();
			components::RenderBufferComponent* p_buffer = entity.getComponent<components::RenderBufferComponent>();

			p_mgr->mgr.BeginUpload();
			p_mgr->mgr.UploadPerInstanceData(p_buffer->buffer.GetStartAddress(), p_buffer->buffer.GetUsedMemory(), 0);
		}





		PipelineShadowMapSystem::PipelineShadowMapSystem()
		{
			updateType = EntityUpdate;
			typeFilter.addRequirement(components::RenderManagerComponent::typeID);
			typeFilter.addRequirement(components::PipelineShadowMapComponent::typeID);
		}

		void PipelineShadowMapSystem::updateEntity(FilteredEntity& entity, float delta)
		{
			components::RenderManagerComponent* p_mgr = entity.getComponent<components::RenderManagerComponent>();
			components::PipelineShadowMapComponent* p_pipeline = entity.getComponent<components::PipelineShadowMapComponent>();

			SetViewMatrix(
				p_pipeline->data.ViewMatrix,
				-8.0f, 10.0f, -2.0f,
				1.2f, -0.7f, 1.0f);

			p_mgr->mgr.UpdatePipeline(p_pipeline->pipeline, &p_pipeline->data);
		}





		PipelineForwardSystem::PipelineForwardSystem()
		{
			updateType = EntityUpdate;
			typeFilter.addRequirement(components::RenderManagerComponent::typeID);
			typeFilter.addRequirement(components::PipelineForwardComponent::typeID);
		}

		void PipelineForwardSystem::updateEntity(FilteredEntity& entity, float delta)
		{
			// Fetch camera entity
			components::CameraComponent* p_camera = getEntitiesWithComponent<components::CameraComponent>().entities.front().getComponent<components::CameraComponent>();

			components::RenderManagerComponent* p_mgr = entity.getComponent<components::RenderManagerComponent>();
			components::PipelineForwardComponent* p_pipeline = entity.getComponent<components::PipelineForwardComponent>();

			p_pipeline->data.ViewMatrix = p_camera->viewMatrix;
			p_mgr->mgr.UpdatePipeline(p_pipeline->pipeline, &p_pipeline->data);
		}


		ClearGPURenderSystem::ClearGPURenderSystem()
		{
			updateType = EntityUpdate;
			typeFilter.addRequirement(components::RenderManagerComponent::typeID);
			typeFilter.addRequirement(components::MeshManagerComponent::typeID);

			typeFilter.addRequirement(components::PipelineShadowMapComponent::typeID);
			typeFilter.addRequirement(components::PipelineForwardComponent::typeID);
		}

		void ClearGPURenderSystem::updateEntity(FilteredEntity& entity, float delta)
		{
			components::MeshManagerComponent* p_mesh_mgr = entity.getComponent<components::MeshManagerComponent>();
			components::RenderManagerComponent* p_render_mgr = entity.getComponent<components::RenderManagerComponent>();

			components::PipelineShadowMapComponent* p_pipeline_shadow_map = entity.getComponent<components::PipelineShadowMapComponent>();
			components::PipelineForwardComponent* p_pipeline_forward = entity.getComponent<components::PipelineForwardComponent>();

			p_render_mgr->mgr.ClearPipeline(p_pipeline_shadow_map->pipeline);
			p_render_mgr->mgr.ClearPipeline(p_pipeline_forward->pipeline);
		}

		ExecuteGPURenderSystem::ExecuteGPURenderSystem()
		{
			updateType = EntityUpdate;
			typeFilter.addRequirement(components::RenderManagerComponent::typeID);
			typeFilter.addRequirement(components::MeshManagerComponent::typeID);

			typeFilter.addRequirement(components::PipelineShadowMapComponent::typeID);
			typeFilter.addRequirement(components::PipelineForwardComponent::typeID);
		}

		void ExecuteGPURenderSystem::updateEntity(FilteredEntity& entity, float delta)
		{
			components::MeshManagerComponent* p_mesh_mgr = entity.getComponent<components::MeshManagerComponent>();
			components::RenderManagerComponent* p_render_mgr = entity.getComponent<components::RenderManagerComponent>();

			components::PipelineShadowMapComponent* p_pipeline_shadow_map = entity.getComponent<components::PipelineShadowMapComponent>();
			components::PipelineForwardComponent* p_pipeline_forward = entity.getComponent<components::PipelineForwardComponent>();

			systems::OceanRenderSystem* p_ocean_renderer = (systems::OceanRenderSystem*)GetSystem<systems::OceanRenderSystem>();

			p_mesh_mgr->mgr.SetVertexBuffers();

#ifdef _DEBUG
			p_render_mgr->mgr.ExecutePipeline(
				p_pipeline_shadow_map->pipeline,
				0,
				0);
#else
			p_render_mgr->mgr.ExecutePipeline(
				p_pipeline_shadow_map->pipeline,
				0,
				p_ocean_renderer->mRenderProgram - 1);
#endif // !_DEBUG


			p_render_mgr->mgr.ExecutePipeline(p_pipeline_forward->pipeline);
		}

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
			for (; p_spawner_component->TimerSinceLastSpawn > p_spawner_component->SpawnFrequency; p_spawner_component->TimerSinceLastSpawn -= p_spawner_component->SpawnFrequency)
			{
				components::ParticleComponent particle;
				components::SmokeParticleComponent smoke;

				particle.Position = p_spawner_component->StartPosition;
				smoke.CurrentLifeDuration = p_spawner_component->LifeDuration;

				particle.Red	= 15;
				particle.Green	= 15;
				particle.Blue	= 15;
				particle.Scale	= 20;

				// Randomize x and y direction

				float x = ((float)rand() / (float)RAND_MAX) - 0.5f;
				float z = ((float)rand() / (float)RAND_MAX) - 0.5f;
				float speed = ((float)rand() / (float)RAND_MAX * 0.9f + 0.1f) * p_smoke_component->InitialVelocity;

				DirectX::XMVECTOR direction = DirectX::XMVectorSet(x, 1.0f, z, 0.0f);
				direction = DirectX::XMVector3Normalize(direction);
				direction = DirectX::XMVectorScale(direction, speed);
				DirectX::XMStoreFloat3(&smoke.Direction, direction);

				createEntity(particle, smoke);
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
			const float gravity = 2.0f;

			components::ParticleComponent* p_particle_component =
				entity.getComponent<components::ParticleComponent>();

			components::SmokeParticleComponent* p_smoke_component =
				entity.getComponent<components::SmokeParticleComponent>();

			p_smoke_component->CurrentLifeDuration -= delta;
			if (p_smoke_component->CurrentLifeDuration <= 0.0f)
			{
				removeEntity(entity.entity->getID());
			}

			DirectX::XMVECTOR position = DirectX::XMLoadFloat3(&p_particle_component->Position);
			DirectX::XMVECTOR direction = DirectX::XMLoadFloat3(&p_smoke_component->Direction);

			position = DirectX::XMVectorAdd(position, DirectX::XMVectorScale(direction, delta));
			direction = DirectX::XMVectorAdd(direction, DirectX::XMVectorSet(0.0f, -gravity * delta, 0.0f, 0.0f));

			DirectX::XMStoreFloat3(&p_particle_component->Position, position);
			DirectX::XMStoreFloat3(&p_smoke_component->Direction, direction);
		}
#pragma endregion SmokeParticleRegion
		//
//#pragma region SnowParticleRegion
//		SnowSpawnerSystem::SnowSpawnerSystem()
//		{
//			updateType = EntityUpdate;
//			typeFilter.addRequirement(components::ParticleSpawnerComponent::typeID);
//		}
//
//		void SnowSpawnerSystem::updateEntity(FilteredEntity& entity, float delta)
//		{
//			components::ParticleSpawnerComponent* p_spawner_component =
//				entity.getComponent<components::ParticleSpawnerComponent>();
//
//			p_spawner_component->TimerSinceLastSpawn += delta;
//			for (; p_spawner_component->TimerSinceLastSpawn > p_spawner_component->SpawnFrequency; p_spawner_component->TimerSinceLastSpawn -= p_spawner_component->SpawnFrequency)
//			{
//				components::ParticleComponent particle;
//
//				particle.Position = p_spawner_component->StartPosition;
//				particle.CurrentLifeDuration = 10;
//
//				particle.Red = 200;
//				particle.Green = 0;
//				particle.Blue = 0;
//				particle.Scale = 20;
//
//				float x = 0.0f;
//				float z = 0.0f;
//				float speed = 0.0f;
//
//				// Randomize x and y direction
//
//				x = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;
//				z = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;
//				speed = ((float)rand() / (float)RAND_MAX + 1.0f) * 10.0f;
//
//				DirectX::XMVECTOR direction = DirectX::XMVectorSet(x, 1.0f, z, 0.0f);
//				direction = DirectX::XMVector3Normalize(direction);
//				direction = DirectX::XMVectorScale(direction, speed);
//				DirectX::XMStoreFloat3(&particle.Direction, direction);
//
//				createEntity(particle);
//			}
//		}
//
//		SnowUpdateSystem::SnowUpdateSystem()
//		{
//			updateType = EntityUpdate;
//			typeFilter.addRequirement(components::ParticleComponent::typeID);
//		}
//
//		void SnowUpdateSystem::updateEntity(FilteredEntity& entity, float delta)
//		{
//			components::ParticleComponent* p_particle_component =
//				entity.getComponent<components::ParticleComponent>();
//
//			p_particle_component->CurrentLifeDuration -= delta;
//			if (p_particle_component->CurrentLifeDuration <= 0.0f)
//			{
//				removeEntity(entity.entity->getID());
//			}
//
//
//			DirectX::XMVECTOR position = DirectX::XMLoadFloat3(&p_particle_component->Position);
//			DirectX::XMVECTOR direction = DirectX::XMLoadFloat3(&p_particle_component->Direction);
//
//			position = DirectX::XMVectorAdd(position, DirectX::XMVectorScale(direction, delta));
//			direction = DirectX::XMVectorAdd(direction, DirectX::XMVectorSet(0.0f, -10.0f * delta, 0.0f, 0.0f));
//
//			DirectX::XMStoreFloat3(&p_particle_component->Position, position);
//			DirectX::XMStoreFloat3(&p_particle_component->Direction, direction);
//		}
//#pragma endregion SnowParticleRegion 
	}
}