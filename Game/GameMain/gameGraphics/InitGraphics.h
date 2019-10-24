#pragma once

#include "ecs.h"
#include "GraphicsECSSystems.h"

using namespace ecs;
using namespace ecs::components;
using namespace ecs::systems;

void InitGraphicsComponents(EntityComponentSystem& rEcs, UINT renderBufferSize, UINT clientWidth, UINT clientHeight)
{
	components::RenderManagerComponent rmDummy;
	components::MeshManagerComponent mmDummy;
	components::RenderBufferComponent rbDummy;
	components::PipelineShadowMapComponent psmDummy;
	components::PipelineForwardComponent pfDummy;


	ID graphics_entity_id = rEcs.createEntity(rmDummy, mmDummy, rbDummy, psmDummy, pfDummy)->getID();

	graphics::RenderManager& renderer = rEcs.getComponentFromEntity<components::RenderManagerComponent>(graphics_entity_id)->mgr;
	graphics::MeshManager& mesh_manager = rEcs.getComponentFromEntity<components::MeshManagerComponent>(graphics_entity_id)->mgr;

	renderer.Initialize(renderBufferSize);
	mesh_manager.Initialize(10000, 10000);

	components::PipelineShadowMapComponent* p_psmComp = rEcs.getComponentFromEntity<components::PipelineShadowMapComponent>(graphics_entity_id);
	components::PipelineForwardComponent* p_pfComp = rEcs.getComponentFromEntity<components::PipelineForwardComponent>(graphics_entity_id);

	p_psmComp->pipelineDesc.PixelsWidth = 2048;
	p_psmComp->pipelineDesc.Width = 45.0f;
	p_psmComp->pipelineDesc.Height = 80.0f;
	p_psmComp->pipelineDesc.NearPlane = 1.0f;
	p_psmComp->pipelineDesc.FarPlane = 40.0f;
	p_psmComp->pipeline = renderer.CreatePipeline(new graphics::ShadowMapPipeline, &p_psmComp->pipelineDesc);

	p_pfComp->pipelineDesc.ClientWidth = clientWidth;
	p_pfComp->pipelineDesc.ClientHeight = clientHeight;
	p_pfComp->pipelineDesc.Fov = 3.14f / 2.0f;
	p_pfComp->pipelineDesc.NearPlane = 0.1f;
	p_pfComp->pipelineDesc.FarPlane = 100.0f;
	p_pfComp->pipeline = renderer.CreatePipeline(new graphics::ForwardRenderingPipeline, &p_pfComp->pipelineDesc);

	components::RenderBufferComponent* p_render_buffer = static_cast<components::RenderBufferComponent*>(rEcs.getAllComponentsOfType(components::RenderBufferComponent::typeID).next());
	p_render_buffer->buffer.Initialize(renderBufferSize, 256);
	p_render_buffer->bufferSize = renderBufferSize;
}

void InitGraphicsPreRenderSystems(EntityComponentSystem& rEcs)
{
	rEcs.createSystem<systems::RenderBufferResetSystem>(0);
}

void InitGraphicsPostRenderSystems(EntityComponentSystem& rEcs)
{
	/*
	-- Initialize Mesh Renderers --
*/
	graphics::RenderManager& render_manager = static_cast<components::RenderManagerComponent*>(rEcs.getAllComponentsOfType(components::RenderManagerComponent::typeID).next())->mgr;
	graphics::RenderBuffer& render_buffer = static_cast<components::RenderBufferComponent*>(rEcs.getAllComponentsOfType(components::RenderBufferComponent::typeID).next())->buffer;

	systems::UnitRenderSystem* p_unit_renderer = rEcs.createSystem<systems::UnitRenderSystem>(9);
	systems::SceneObjectRenderSystem* p_scenery_renderer = rEcs.createSystem<systems::SceneObjectRenderSystem>(9);
	systems::TileRenderSystem* p_tile_renderer = rEcs.createSystem<systems::TileRenderSystem>(9);

	p_unit_renderer->Initialize(&render_manager, &render_buffer);
	p_scenery_renderer->Initialize(&render_manager, &render_buffer);
	p_tile_renderer->Initialize(&render_manager, &render_buffer);

	rEcs.createSystem<systems::UploadGPUDataSystem>(9);
	rEcs.createSystem<systems::PipelineShadowMapSystem>(9);
	rEcs.createSystem<systems::PipelineForwardSystem>(9);
	rEcs.createSystem<systems::ExecuteGPURenderSystem>(9);
}

void InitMeshes(EntityComponentSystem& rEcs)
{
	graphics::MeshManager& mesh_manager = static_cast<components::MeshManagerComponent*>(rEcs.getAllComponentsOfType(components::MeshManagerComponent::typeID).next())->mgr;
	MeshContainer::Initialize(&mesh_manager);
	MeshContainer::LoadMesh(MESH_TYPE_TILE, "../meshes/hexagon_tile5.fbx");
	MeshContainer::LoadMesh(MESH_TYPE_ROCK, "../meshes/rock.fbx");
	MeshContainer::LoadMesh(MESH_TYPE_TREE, "../meshes/tree2.fbx");
	MeshContainer::LoadMesh(MESH_TYPE_UNIT, "../RunningCustom2.fbx");
}