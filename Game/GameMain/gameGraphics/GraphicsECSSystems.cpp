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
				0.5f, -1.0f, 0.5f);

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
			typeFilter.addRequirement(components::PipelineDepthPrePassComponent::typeID);
			typeFilter.addRequirement(components::PipelineForwardComponent::typeID);
		}

		void ClearGPURenderSystem::updateEntity(FilteredEntity& entity, float delta)
		{
			components::MeshManagerComponent* p_mesh_mgr = entity.getComponent<components::MeshManagerComponent>();
			components::RenderManagerComponent* p_render_mgr = entity.getComponent<components::RenderManagerComponent>();

			components::PipelineShadowMapComponent* p_pipeline_shadow_map = entity.getComponent<components::PipelineShadowMapComponent>();
			components::PipelineDepthPrePassComponent* p_pipeline_depth_pre_pass = entity.getComponent<components::PipelineDepthPrePassComponent>();
			components::PipelineForwardComponent* p_pipeline_forward = entity.getComponent<components::PipelineForwardComponent>();

			p_render_mgr->mgr.ClearPipeline(p_pipeline_shadow_map->pipeline);
			p_render_mgr->mgr.ClearPipeline(p_pipeline_depth_pre_pass->pipeline);
			p_render_mgr->mgr.ClearPipeline(p_pipeline_forward->pipeline);
		}


		ExecuteGPURenderSystem::ExecuteGPURenderSystem()
		{
			updateType = EntityUpdate;
			typeFilter.addRequirement(components::RenderManagerComponent::typeID);
			typeFilter.addRequirement(components::MeshManagerComponent::typeID);

			typeFilter.addRequirement(components::PipelineShadowMapComponent::typeID);
			typeFilter.addRequirement(components::PipelineDepthPrePassComponent::typeID);
			typeFilter.addRequirement(components::PipelineForwardComponent::typeID);
		}

		void ExecuteGPURenderSystem::updateEntity(FilteredEntity& entity, float delta)
		{
			components::MeshManagerComponent* p_mesh_mgr = entity.getComponent<components::MeshManagerComponent>();
			components::RenderManagerComponent* p_render_mgr = entity.getComponent<components::RenderManagerComponent>();

			components::PipelineShadowMapComponent* p_pipeline_shadow_map = entity.getComponent<components::PipelineShadowMapComponent>();
			components::PipelineDepthPrePassComponent* p_pipeline_depth_pre_pass = entity.getComponent<components::PipelineDepthPrePassComponent>();
			components::PipelineForwardComponent* p_pipeline_forward = entity.getComponent<components::PipelineForwardComponent>();

			systems::OceanRenderSystem* p_ocean_renderer = (systems::OceanRenderSystem*)GetSystem<systems::OceanRenderSystem>();

			// Enable Standard Vertex Buffers
			p_mesh_mgr->mgr.SetVertexBuffers();

#ifdef _DEBUG
			// Don't render shadow map if in debug (but execute the begin and end for the pipeline)
			p_render_mgr->mgr.ExecutePipeline(
				p_pipeline_shadow_map->pipeline,
				0,
				0);
#else
			// Render To Shadow Map
			p_render_mgr->mgr.ExecutePipeline(
				p_pipeline_shadow_map->pipeline,
				0,
				p_ocean_renderer->mRenderProgram - 1);
#endif // !_DEBUG

			// Render To Color Buffer
			//p_render_mgr->mgr.ExecutePipeline(p_pipeline_forward->pipeline, 0, 0);

			// Render To Depth Buffer (Depth Pre Pass)
			p_render_mgr->mgr.ExecutePipeline(p_pipeline_depth_pre_pass->pipeline);

			// Render To Color Buffer
			p_render_mgr->mgr.ExecutePipeline(p_pipeline_forward->pipeline);
			
		}
	}
}