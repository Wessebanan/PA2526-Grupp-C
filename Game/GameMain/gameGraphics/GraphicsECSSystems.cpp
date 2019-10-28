#include "GraphicsECSSystems.h"
#include "../gameUtility/CameraComponents.h"

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
				6.0f, 4.0f, -5.0f,
				1.0f, -1.0f, 1.0f);

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
			p_pipeline->data.Red = 0.25f;
			p_pipeline->data.Green = 0.25f;
			p_pipeline->data.Blue = 1.0f;

			p_mgr->mgr.UpdatePipeline(p_pipeline->pipeline, &p_pipeline->data);
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

			p_mesh_mgr->mgr.SetVertexBuffers();
			p_render_mgr->mgr.ExecutePipeline(p_pipeline_shadow_map->pipeline);
			p_render_mgr->mgr.ExecutePipeline(p_pipeline_forward->pipeline);
		}
	}
}