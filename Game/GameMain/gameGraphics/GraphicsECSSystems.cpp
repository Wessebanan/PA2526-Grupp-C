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
				6.0f, 10.0f, 8.0f,
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
			typeFilter.addRequirement(components::PipelineOutlineComponent::typeID);
			typeFilter.addRequirement(components::PipelineForwardComponent::typeID);
		}

		void ExecuteGPURenderSystem::updateEntity(FilteredEntity& entity, float delta)
		{
			components::MeshManagerComponent* p_mesh_mgr = entity.getComponent<components::MeshManagerComponent>();
			components::RenderManagerComponent* p_render_mgr = entity.getComponent<components::RenderManagerComponent>();

			components::PipelineShadowMapComponent* p_pipeline_shadow_map = entity.getComponent<components::PipelineShadowMapComponent>();
			components::PipelineDepthPrePassComponent* p_pipeline_depth_pre_pass = entity.getComponent<components::PipelineDepthPrePassComponent>();
			components::PipelineOutlineComponent* p_pipeline_outline = entity.getComponent<components::PipelineOutlineComponent>();
			components::PipelineForwardComponent* p_pipeline_forward = entity.getComponent<components::PipelineForwardComponent>();

			// Get ocean render system to make it the first renderer in order to not cast shadow
			systems::OceanRenderSystem* p_ocean_renderer = (systems::OceanRenderSystem*)GetSystem<systems::OceanRenderSystem>();

			// Set all to cast shadows
			UINT first_shader_program_index_to_not_cast_shadow = UINT_MAX;
			if (p_ocean_renderer)
			{
				// if ocean renderer exists, then make ocean the first program to not cast shadow
				first_shader_program_index_to_not_cast_shadow = p_ocean_renderer->mRenderProgram - 1;
			}

			// Get unit and weapon renderer to make sure in between them outlines are made for them
			systems::WeaponRenderSystem* p_weapon_renderer = (systems::WeaponRenderSystem*)GetSystem<systems::WeaponRenderSystem>();
			systems::UnitRenderSystem* p_unit_renderer = (systems::UnitRenderSystem*)GetSystem<systems::UnitRenderSystem>();


			UINT first_shader_program_index_to_draw_outline = UINT_MAX;
			UINT last_shader_program_index_to_draw_outline = UINT_MAX;

			if (p_weapon_renderer && p_unit_renderer)
			{
				first_shader_program_index_to_draw_outline = p_weapon_renderer->mRenderProgram;
				last_shader_program_index_to_draw_outline = p_unit_renderer->mRenderProgram;
			}
			


			// Enable Standard Vertex Buffers
			p_mesh_mgr->mgr.SetVertexBuffers();

			// Render To Shadow Map
			p_render_mgr->mgr.ExecutePipeline(
				p_pipeline_shadow_map->pipeline,
				0,
				first_shader_program_index_to_not_cast_shadow);

			// Render Depth Buffer (Depth Pre Pass)
			p_render_mgr->mgr.ExecutePipeline(p_pipeline_depth_pre_pass->pipeline);

			// Render Scene
			p_render_mgr->mgr.ExecutePipeline(p_pipeline_forward->pipeline);

			// Render Outline
			p_render_mgr->mgr.ExecutePipeline(
				p_pipeline_outline->pipeline,
				first_shader_program_index_to_draw_outline,
				last_shader_program_index_to_draw_outline);
		}
	}
}