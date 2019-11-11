#pragma once

#include "ecs.h"
#include "GraphicsECSSystems.h"
#include "../Renderers/Renderers.h"

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
	/*
		Specify that we only need to reserve memory for a single component for all
		'singleton' graphic components.
	*/

	rEcs.reserveComponentCount<RenderManagerComponent>(1);
	rEcs.reserveComponentCount<MeshManagerComponent>(1);
	rEcs.reserveComponentCount<StateManagerComponent>(1);

	rEcs.reserveComponentCount<RenderBufferComponent>(1);
	rEcs.reserveComponentCount<PipelineShadowMapComponent>(1);
	rEcs.reserveComponentCount<PipelineForwardComponent>(1);

	/*
		To create an entity we need some component info, but all these have their
		own Initialize() so we create empty components and fill them after
		creation when they are in the correct ECS memory.
	*/

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
	list.componentCount = sizeof(graphics_components) / sizeof(BaseComponent*);

	/*
		Fetch id of the graphics entity. All graphic components used by renderer systems
		are attached to this entity for easy access. Nice!
	*/

	ID graphics_entity_id = rEcs.createEntity(list)->getID();

	/*
		Initialize all 'singleton' graphic components.
	*/

	graphics::RenderManager& r_renderer_mgr = rEcs.getComponentFromEntity<components::RenderManagerComponent>(graphics_entity_id)->mgr;
	graphics::MeshManager& r_mesh_mgr = rEcs.getComponentFromEntity<components::MeshManagerComponent>(graphics_entity_id)->mgr;
	graphics::StateManager& r_state_mgr = rEcs.getComponentFromEntity<components::StateManagerComponent>(graphics_entity_id)->mgr;

	r_renderer_mgr.Initialize(renderBufferSize);
	r_mesh_mgr.Initialize(1000000, 1000000);	// Max vertex count and intance count! We can't render more than we specify here.
	r_state_mgr.Initialize();

	components::PipelineShadowMapComponent* p_psmComp = rEcs.getComponentFromEntity<components::PipelineShadowMapComponent>(graphics_entity_id);
	components::PipelineForwardComponent* p_pfComp = rEcs.getComponentFromEntity<components::PipelineForwardComponent>(graphics_entity_id);

	p_psmComp->pipelineDesc.PixelsWidth = 2048;
	p_psmComp->pipelineDesc.Width = 80.0f;
	p_psmComp->pipelineDesc.Height = 80.0f;
	p_psmComp->pipelineDesc.NearPlane = 1.0f;
	p_psmComp->pipelineDesc.FarPlane = 80.0f;
	p_psmComp->pipeline = r_renderer_mgr.CreatePipeline(new graphics::ShadowMapPipeline, &p_psmComp->pipelineDesc);

	p_pfComp->pipelineDesc.ClientWidth = clientWidth;
	p_pfComp->pipelineDesc.ClientHeight = clientHeight;
	p_pfComp->pipelineDesc.Fov = CameraDefines::fovAngle;
	p_pfComp->pipelineDesc.NearPlane = CameraDefines::nearPlane;
	p_pfComp->pipelineDesc.FarPlane = CameraDefines::farPlane;

	p_pfComp->pipeline = r_renderer_mgr.CreatePipeline(new graphics::ForwardRenderingPipeline, &p_pfComp->pipelineDesc);

	components::RenderBufferComponent* p_render_buffer = static_cast<components::RenderBufferComponent*>(rEcs.getAllComponentsOfType(components::RenderBufferComponent::typeID).next());
	p_render_buffer->buffer.Initialize(renderBufferSize, 256);
	p_render_buffer->bufferSize = renderBufferSize;
}

void InitGraphicsPreRenderSystems(EntityComponentSystem& rEcs)
{
	rEcs.createSystem<systems::RenderBufferResetSystem>(0);
}

void InitGraphicsRenderSystems(EntityComponentSystem& rEcs, WorldMeshData& worldMeshData, const UINT clientWidth, const UINT clientHeight)
{
	/*
		Fetch all renderers from the ECS memory and Initialize() them.
		They like to be initialized, this is how we keep these monsters tamed.
	*/

	graphics::RenderManager& r_render_mgr = static_cast<components::RenderManagerComponent*>(rEcs.getAllComponentsOfType(components::RenderManagerComponent::typeID).next())->mgr;
	graphics::StateManager& r_state_mgr = static_cast<components::StateManagerComponent*>(rEcs.getAllComponentsOfType(components::StateManagerComponent::typeID).next())->mgr;
	graphics::MeshManager& r_mesh_mgr = static_cast<components::MeshManagerComponent*>(rEcs.getAllComponentsOfType(components::MeshManagerComponent::typeID).next())->mgr;
	graphics::RenderBuffer& r_render_buffer = static_cast<components::RenderBufferComponent*>(rEcs.getAllComponentsOfType(components::RenderBufferComponent::typeID).next())->buffer;

	systems::UnitRenderSystem* p_unit_renderer = rEcs.createSystem<systems::UnitRenderSystem>(9);
	systems::SceneObjectRenderSystem* p_scenery_renderer = rEcs.createSystem<systems::SceneObjectRenderSystem>(9);	
	systems::SwordRenderSystem* p_sword_renderer = rEcs.createSystem<systems::SwordRenderSystem>(9);
	systems::WorldRenderSystem* p_world_renderer = rEcs.createSystem<WorldRenderSystem>(9);

	
	p_unit_renderer->Initialize(&r_render_mgr, &r_render_buffer);
	p_scenery_renderer->Initialize(&r_render_mgr, &r_render_buffer);
	p_sword_renderer->Initialize(&r_render_mgr, &r_render_buffer);
	p_world_renderer->Initialize(&r_render_mgr, &r_state_mgr, worldMeshData.pMesh, worldMeshData.vertexCount);

	systems::SSAORenderSystem* p_ssao_renderer = rEcs.createSystem<systems::SSAORenderSystem>(9);
	p_ssao_renderer->Initialize(&r_mesh_mgr, clientWidth, clientHeight);

	/*
		These stay outcommented, so we can easily compare performance boost between instance and single mesh rendering
	*/

	//systems::TileRenderSystem* p_tile_renderer = rEcs.createSystem<systems::TileRenderSystem>(9);
	//systems::OceanRenderSystem* p_ocean_renderer = rEcs.createSystem<systems::OceanRenderSystem>(9);
	//p_tile_renderer->Initialize(&r_render_mgr, &r_render_buffer);
	//p_ocean_renderer->Initialize(&r_render_mgr, &r_render_buffer);
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
	/*
		In this function, we load ALL meshes we would like to render in the game.
		Later, when we want to use them, we simply call

			graphics::MeshManager::GetMeshCPU(MESH_TYPE);
	*/

	graphics::MeshManager& mesh_manager = static_cast<components::MeshManagerComponent*>(rEcs.getAllComponentsOfType(components::MeshManagerComponent::typeID).next())->mgr;
	MeshContainer::Initialize(&mesh_manager);
	MeshContainer::LoadMesh(GAME_OBJECT_TYPE_TILE, "../meshes/hexagon_tile5.fbx");
	MeshContainer::LoadMesh(GAME_OBJECT_TYPE_BARREL, "../meshes/Barrel.fbx");
	MeshContainer::LoadMesh(GAME_OBJECT_TYPE_BOX, "../meshes/Box.fbx");
	MeshContainer::LoadMesh(GAME_OBJECT_TYPE_CACTUS, "../meshes/Cactus.fbx");
	MeshContainer::LoadMesh(GAME_OBJECT_TYPE_CAGE, "../meshes/Cage.fbx");
	MeshContainer::LoadMesh(GAME_OBJECT_TYPE_COWSKULL, "../meshes/CowSkull.fbx");
	MeshContainer::LoadMesh(GAME_OBJECT_TYPE_FRUITTREE, "../meshes/FruitTree.fbx");
	MeshContainer::LoadMesh(GAME_OBJECT_TYPE_GIANTSKULL, "../meshes/GiantSkull.fbx");
	MeshContainer::LoadMesh(GAME_OBJECT_TYPE_TOWER, "../meshes/Tower.fbx");
	MeshContainer::LoadMesh(GAME_OBJECT_TYPE_WINTERTREE, "../meshes/WinterTree.fbx");
	MeshContainer::LoadMesh(GAME_OBJECT_TYPE_UNIT, "../DudeMesh3.fbx");
	MeshContainer::LoadMesh(GAME_OBJECT_TYPE_SWORD, "../meshes/sword.fbx");
}