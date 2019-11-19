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
	list.componentCount = 6;

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

	p_psmComp->pipelineDesc.PixelsWidth = 4096;
	p_psmComp->pipelineDesc.Width = 90.0f;
	p_psmComp->pipelineDesc.Height = 60.0f;
	p_psmComp->pipelineDesc.NearPlane = 1.0f;
	p_psmComp->pipelineDesc.FarPlane = 70.0f;
	p_psmComp->pipeline = r_renderer_mgr.CreatePipeline(new graphics::ShadowMapPipeline, &p_psmComp->pipelineDesc);

	p_pfComp->pipelineDesc.ClientWidth = clientWidth;
	p_pfComp->pipelineDesc.ClientHeight = clientHeight;
	p_pfComp->pipelineDesc.Fov = 3.14f / 2.0f;
	p_pfComp->pipelineDesc.NearPlane = 1.0f;
	p_pfComp->pipelineDesc.FarPlane = 100.0f;
	p_pfComp->pipelineDesc.ClearColor[0] = 0.25f;
	p_pfComp->pipelineDesc.ClearColor[1] = 0.25f;
	p_pfComp->pipelineDesc.ClearColor[2] = 1.00f;
	p_pfComp->pipeline = r_renderer_mgr.CreatePipeline(new graphics::ForwardRenderingPipeline, &p_pfComp->pipelineDesc);

	components::RenderBufferComponent* p_render_buffer = static_cast<components::RenderBufferComponent*>(rEcs.getAllComponentsOfType(components::RenderBufferComponent::typeID).next());
	p_render_buffer->buffer.Initialize(renderBufferSize, 256);
	p_render_buffer->bufferSize = renderBufferSize;
}

void InitGraphicsPreRenderSystems(EntityComponentSystem& rEcs)
{
	rEcs.createSystem<systems::RenderBufferResetSystem>(0);
	rEcs.createSystem<systems::ClearGPURenderSystem>(0);
}

void InitGraphicsRenderSystems(EntityComponentSystem& rEcs, WorldMeshData& rMapMeshData, WorldMeshData& rOceanMeshData, const UINT clientWidth, const UINT clientHeight)
{
	/*
		Fetch all renderers from the ECS memory and Initialize() them.
		They like to be initialized, this is how we keep these monsters tamed.
	*/

	graphics::RenderManager& r_render_mgr = static_cast<components::RenderManagerComponent*>(rEcs.getAllComponentsOfType(components::RenderManagerComponent::typeID).next())->mgr;
	graphics::StateManager& r_state_mgr = static_cast<components::StateManagerComponent*>(rEcs.getAllComponentsOfType(components::StateManagerComponent::typeID).next())->mgr;
	graphics::MeshManager& r_mesh_mgr = static_cast<components::MeshManagerComponent*>(rEcs.getAllComponentsOfType(components::MeshManagerComponent::typeID).next())->mgr;
	graphics::RenderBuffer& r_render_buffer = static_cast<components::RenderBufferComponent*>(rEcs.getAllComponentsOfType(components::RenderBufferComponent::typeID).next())->buffer;
	rEcs.createSystem<TrapRenderSystem>(9)->Initialize(&r_render_mgr, &r_render_buffer);

	rEcs.createSystem<systems::UnitRenderSystem>(9)
		->Initialize(&r_render_mgr, &r_render_buffer);

	rEcs.createSystem<systems::SceneObjectRenderSystem>(9)
		->Initialize(&r_render_mgr, &r_render_buffer);

	rEcs.createSystem<systems::WeaponRenderSystem>(9)
		->Initialize(&r_render_mgr, &r_render_buffer);

	rEcs.createSystem<MapRenderSystem>(9)
		->Initialize(&r_render_mgr, &r_state_mgr, 
			rMapMeshData.pMesh, 
			rMapMeshData.vertexCount);

	rEcs.createSystem<ParticleRenderSystem>(9)
		->Initialize(&r_render_mgr, &r_render_buffer, &r_state_mgr);

	rEcs.createSystem<OceanRenderSystem>(9)
		->Initialize(&r_render_mgr, &r_state_mgr, 
			rOceanMeshData.pMesh,
			rOceanMeshData.vertexCount);


	/*
		These stay outcommented, so we can easily compare performance boost between instance and single mesh rendering
	*/

	//rEcs.createSystem<systems::TileInstanceRenderSystem>(9)->Initialize(&r_render_mgr, &r_render_buffer);
	//systems::OceanRenderSystem* p_ocean_renderer = rEcs.createSystem<systems::OceanRenderSystem>(9);
	//p_ocean_renderer->Initialize(&r_render_mgr, &r_render_buffer);
}

void InitGraphicsPostRenderSystems(EntityComponentSystem& rEcs)
{
	rEcs.createSystem<systems::UploadRenderBufferSystem>(9);
	rEcs.createSystem<systems::PipelineShadowMapSystem>(9);
	rEcs.createSystem<systems::PipelineForwardSystem>(9);
	rEcs.createSystem<systems::ExecuteGPURenderSystem>(9);
	rEcs.createSystem<systems::SSAORenderSystem>(9)->Initialize(graphics::GetDisplayResolution().x, graphics::GetDisplayResolution().y);
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

	MeshContainer::LoadMesh(GAME_OBJECT_TYPE_WEAPON_SWORD, "../meshes/sword.fbx");

	// Create Quad For GPU
	{
		struct float3
		{
			float x, y, z;
		};

		struct float2
		{
			float x, y;
		};

		float3 vertices[6] =
		{
			-1.0f, -1.0f, 0.5f,
			-1.0f,  1.0f, 0.5f,
			 1.0f, -1.0f, 0.5f,

			 1.0f, -1.0f, 0.5f,
			-1.0f,  1.0f, 0.5f,
			 1.0f,  1.0f, 0.5f,
		};

		float2 uv[6] =
		{
			0.0f, 1.0f,
			0.0f, 0.0f,
			1.0f, 1.0f,

			1.0f, 1.0f,
			0.0f, 0.0f,
			1.0f, 0.0f,
		};

		graphics::VERTEX_DATA data = { NULL };
		data.pVertexPositions = vertices;
		data.pVertexTexCoords = uv;

		MeshContainer::CreateGPUMesh(GAME_OBJECT_TYPE_QUAD, 6, 0, data, NULL);
	}

	MeshContainer::LoadMesh(GAME_OBJECT_TYPE_TRAP_FIRE, "../meshes/TrapPlate.fbx");
	MeshContainer::LoadMesh(GAME_OBJECT_TYPE_TRAP_FREEZE, "../meshes/TrapPlate.fbx");
	MeshContainer::LoadMesh(GAME_OBJECT_TYPE_TRAP_SPRING, "../meshes/TrapPlate.fbx");

	MeshContainer::LoadMesh(GAME_OBJECT_TYPE_BARREL_STONES, "../meshes/barrel_rock.fbx");
	MeshContainer::LoadMesh(GAME_OBJECT_TYPE_BARREL_BARREL, "../meshes/Barrel.fbx");

	MeshContainer::LoadMesh(GAME_OBJECT_TYPE_TREE_LEAVES, "../meshes/tree_leaves.fbx");
	MeshContainer::LoadMesh(GAME_OBJECT_TYPE_TREE_TRUNK, "../meshes/tree_trunk.fbx");
	MeshContainer::LoadMesh(GAME_OBJECT_TYPE_TREE_ROCK, "../meshes/tree_rock.fbx");
}