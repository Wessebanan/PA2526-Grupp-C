#pragma once

#include "ecs.h"
#include "GraphicsECSSystems.h"

struct WorldMeshData
{
	void* pMesh;
	UINT vertexCount;
};

using namespace ecs;
using namespace ecs::components;
using namespace ecs::systems;

void InitGraphicsComponents(EntityComponentSystem& rEcs, UINT renderBufferSize, UINT clientWidth, UINT clientHeight)
{
	rEcs.reserveComponentCount<RenderManagerComponent>(1);
	rEcs.reserveComponentCount<MeshManagerComponent>(1);
	rEcs.reserveComponentCount<StateManagerComponent>(1);

	rEcs.reserveComponentCount<RenderBufferComponent>(1);
	rEcs.reserveComponentCount<PipelineShadowMapComponent>(1);
	rEcs.reserveComponentCount<PipelineForwardComponent>(1);

	components::RenderManagerComponent rmDummy;
	components::MeshManagerComponent mmDummy;
	components::StateManagerComponent smDummy;

	components::RenderBufferComponent rbDummy;
	components::PipelineShadowMapComponent psmDummy;
	components::PipelineForwardComponent pfDummy;

	BaseComponent* graphics_components[] =
	{
		&rmDummy,
		&mmDummy,
		&smDummy,

		&rbDummy,
		&psmDummy,
		&pfDummy
	};

	ComponentList list;
	list.initialInfo = graphics_components;
	list.componentCount = 6;

	ID graphics_entity_id = rEcs.createEntity(list)->getID();

	graphics::RenderManager& r_renderer_mgr = rEcs.getComponentFromEntity<components::RenderManagerComponent>(graphics_entity_id)->mgr;
	graphics::MeshManager& r_mesh_mgr = rEcs.getComponentFromEntity<components::MeshManagerComponent>(graphics_entity_id)->mgr;
	graphics::StateManager& r_state_mgr = rEcs.getComponentFromEntity<components::StateManagerComponent>(graphics_entity_id)->mgr;

	r_renderer_mgr.Initialize(renderBufferSize);
	r_mesh_mgr.Initialize(1000000, 1000000);
	r_state_mgr.Initialize();

	components::PipelineShadowMapComponent* p_psmComp = rEcs.getComponentFromEntity<components::PipelineShadowMapComponent>(graphics_entity_id);
	components::PipelineForwardComponent* p_pfComp = rEcs.getComponentFromEntity<components::PipelineForwardComponent>(graphics_entity_id);

	p_psmComp->pipelineDesc.PixelsWidth = 2048;
	p_psmComp->pipelineDesc.Width = 45.0f;
	p_psmComp->pipelineDesc.Height = 80.0f;
	p_psmComp->pipelineDesc.NearPlane = 1.0f;
	p_psmComp->pipelineDesc.FarPlane = 40.0f;
	p_psmComp->pipeline = r_renderer_mgr.CreatePipeline(new graphics::ShadowMapPipeline, &p_psmComp->pipelineDesc);

	p_pfComp->pipelineDesc.ClientWidth = clientWidth;
	p_pfComp->pipelineDesc.ClientHeight = clientHeight;
	p_pfComp->pipelineDesc.Fov = 3.14f / 2.0f;
	p_pfComp->pipelineDesc.NearPlane = 0.1f;
	p_pfComp->pipelineDesc.FarPlane = 100.0f;
	p_pfComp->pipeline = r_renderer_mgr.CreatePipeline(new graphics::ForwardRenderingPipeline, &p_pfComp->pipelineDesc);

	components::RenderBufferComponent* p_render_buffer = static_cast<components::RenderBufferComponent*>(rEcs.getAllComponentsOfType(components::RenderBufferComponent::typeID).next());
	p_render_buffer->buffer.Initialize(renderBufferSize, 256);
	p_render_buffer->bufferSize = renderBufferSize;
}

void InitGraphicsPreRenderSystems(EntityComponentSystem& rEcs)
{
	rEcs.createSystem<systems::RenderBufferResetSystem>(0);
}

void InitGraphicsRenderSystems(EntityComponentSystem& rEcs, WorldMeshData& worldMeshData)
{
	graphics::RenderManager& r_render_mgr = static_cast<components::RenderManagerComponent*>(rEcs.getAllComponentsOfType(components::RenderManagerComponent::typeID).next())->mgr;
	graphics::StateManager& r_state_mgr = static_cast<components::StateManagerComponent*>(rEcs.getAllComponentsOfType(components::StateManagerComponent::typeID).next())->mgr;
	graphics::RenderBuffer& r_render_buffer = static_cast<components::RenderBufferComponent*>(rEcs.getAllComponentsOfType(components::RenderBufferComponent::typeID).next())->buffer;

	systems::UnitRenderSystem* p_unit_renderer = rEcs.createSystem<systems::UnitRenderSystem>(9);
	systems::SceneObjectRenderSystem* p_scenery_renderer = rEcs.createSystem<systems::SceneObjectRenderSystem>(9);
	//systems::TileRenderSystem* p_tile_renderer = rEcs.createSystem<systems::TileRenderSystem>(9);
	//systems::OceanRenderSystem* p_ocean_renderer = rEcs.createSystem<systems::OceanRenderSystem>(9);
	WorldRenderSystem* p_world_renderer = rEcs.createSystem<WorldRenderSystem>(9);

	p_unit_renderer->Initialize(&r_render_mgr, &r_render_buffer);
	p_scenery_renderer->Initialize(&r_render_mgr, &r_render_buffer);
	//p_tile_renderer->Initialize(&r_render_mgr, &r_render_buffer);
	//p_ocean_renderer->Initialize(&r_render_mgr, &r_render_buffer);
	p_world_renderer->Initialize(&r_render_mgr, &r_state_mgr, worldMeshData.pMesh, worldMeshData.vertexCount);
}

void InitGraphicsPostRenderSystems(EntityComponentSystem& rEcs)
{
	rEcs.createSystem<systems::UploadRenderBufferSystem>(9);
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