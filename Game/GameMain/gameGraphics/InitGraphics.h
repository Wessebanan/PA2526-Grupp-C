#pragma once

#include "ecs.h"
#include "GraphicsECSSystems.h"
#include "../Renderers/Renderers.h"
#include "..//gameUtility/GlobalsCamera.h"


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
	components::PipelineDepthPrePassComponent pdppDummy;
	components::PipelineOutlineComponent poDummy;
	components::PipelineForwardComponent pfDummy;

	BaseComponent* graphics_components[] =
	{
		&rmDummy,
		&mmDummy,
		&smDummy,

		&rbDummy,
		&psmDummy,
		&pdppDummy,
		&poDummy,
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
	components::PipelineDepthPrePassComponent* p_pdppComp = rEcs.getComponentFromEntity<components::PipelineDepthPrePassComponent>(graphics_entity_id);
	components::PipelineOutlineComponent* p_poComp = rEcs.getComponentFromEntity<components::PipelineOutlineComponent>(graphics_entity_id);

	p_psmComp->pipelineDesc.PixelsWidth = 2048;
	p_psmComp->pipelineDesc.Width = 30.0f;
	p_psmComp->pipelineDesc.Height = 30.0f;
	p_psmComp->pipelineDesc.NearPlane = 1.0f;
	p_psmComp->pipelineDesc.FarPlane = 48.0f;
	p_psmComp->pipeline = r_renderer_mgr.CreatePipeline(new graphics::ShadowMapPipeline, &p_psmComp->pipelineDesc);

	p_pfComp->pipelineDesc.ClientWidth = clientWidth;
	p_pfComp->pipelineDesc.ClientHeight = clientHeight;
	p_pfComp->pipelineDesc.Fov = CameraDefines::fovAngle;
	p_pfComp->pipelineDesc.NearPlane = CameraDefines::nearPlane;
	p_pfComp->pipelineDesc.FarPlane = CameraDefines::farPlane;
	p_pfComp->pipelineDesc.ClearColor[0] = 0.45f;
	p_pfComp->pipelineDesc.ClearColor[1] = 0.35f;
	p_pfComp->pipelineDesc.ClearColor[2] = 1.00f;

	//p_pfComp->pipelineDesc.ClearColor[0] = 255.f / 255.f;
	//p_pfComp->pipelineDesc.ClearColor[1] = 127.f / 255.f;
	//p_pfComp->pipelineDesc.ClearColor[2] = 80.f / 255.f;

	p_pfComp->pipeline = r_renderer_mgr.CreatePipeline(new graphics::ForwardRenderingPipeline, &p_pfComp->pipelineDesc);

	p_pdppComp->pipelineDesc.ClientWidth = clientWidth;
	p_pdppComp->pipelineDesc.ClientHeight = clientHeight;
	p_pdppComp->pipelineDesc.pDepthBuffer = p_pfComp->pipelineDesc.pDepthBuffer;
	p_pdppComp->pipelineDesc.pViewMatrixBuffer = p_pfComp->pipelineDesc.pViewMatrixBuffer;
	p_pdppComp->pipelineDesc.pProjMatrixBuffer = p_pfComp->pipelineDesc.pProjMatrixBuffer;
	p_pdppComp->pipelineDesc.pInvProjMatrixBuffer = p_pfComp->pipelineDesc.pInvProjMatrixBuffer;
	p_pdppComp->pipeline = r_renderer_mgr.CreatePipeline(new graphics::DepthPrePassPipeline, &p_pdppComp->pipelineDesc);


	p_poComp->pipelineDesc.ClientWidth = clientWidth;
	p_poComp->pipelineDesc.ClientHeight = clientHeight;
	p_poComp->pipeline = r_renderer_mgr.CreatePipeline(new graphics::FakeStencilPipeline, &p_poComp->pipelineDesc);


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

	// Make sure no render system is created before UnitRenderSystem if they use the same constant buffer
	// That will cause outlines to break suuuper hard
	// No touch >:(

	/* !!NOTE!!: Everything between 'WeaponRenderSystem' and 'UnitRenderSystem' will receive outline */
	rEcs.createSystem<systems::WeaponRenderSystem>(9)
		->Initialize(&r_render_mgr, &r_render_buffer);


	rEcs.createSystem<systems::UnitRenderSystem>(9)
		->Initialize(&r_render_mgr, &r_render_buffer);
	/* !!NOTE!!: Everything between 'WeaponRenderSystem' and 'UnitRenderSystem' will receive outline */

	rEcs.createSystem<ParticleRenderSystem>(9)
		->Initialize(&r_render_mgr, &r_render_buffer, &r_state_mgr);

	rEcs.createSystem<TrapRenderSystem>(9)->Initialize(&r_render_mgr, &r_render_buffer);

	rEcs.createSystem<systems::SceneObjectRenderSystem>(9)
		->Initialize(&r_render_mgr, &r_render_buffer);

	rEcs.createSystem<MapRenderSystem>(9)
		->Initialize(&r_render_mgr, &r_state_mgr, 
			rMapMeshData.pMesh, 
			rMapMeshData.vertexCount);

	rEcs.createSystem<DefaultRenderSystem>(9)
		->Initialize(&r_render_mgr, &r_render_buffer);

	rEcs.createSystem<PowerupLootRenderSystem>(9)
		->Initialize(&r_render_mgr, &r_render_buffer);

	/* For some reason this doesn't work
	rEcs.createSystem<PowerupLootRenderSystem>(9)
		->Initialize(&r_render_mgr, &r_render_buffer);*/

	rEcs.createSystem<OceanRenderSystem>(9)
		->Initialize(&r_render_mgr, &r_state_mgr,
			rOceanMeshData.pMesh,
			rOceanMeshData.vertexCount);

	rEcs.createSystem<WorldSceneRenderSystem>(9)
		->Initialize(&r_render_mgr, &r_render_buffer);


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

	//UnitRenderSystem* p_unit_system = (UnitRenderSystem*)rEcs.getSystem<UnitRenderSystem>();
	//WeaponRenderSystem* p_weapon_system = (WeaponRenderSystem*)rEcs.getSystem<WeaponRenderSystem>();

	//rEcs.createSystem<systems::OutlineRenderSystem>(9)
	//	->Initialize(graphics::GetDisplayResolution().x, graphics::GetDisplayResolution().y
	//		, p_unit_system->mRenderProgram, p_weapon_system->mRenderProgram,
	//		&static_cast<components::RenderManagerComponent*>(rEcs.getAllComponentsOfType(components::RenderManagerComponent::typeID).next())->mgr);
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
	MeshContainer::LoadMesh(GAME_OBJECT_TYPE_GIANTSKULL, "../meshes/crystal_formation.fbx");
	MeshContainer::LoadMesh(GAME_OBJECT_TYPE_TOWER, "../meshes/Tower.fbx");
	MeshContainer::LoadMesh(GAME_OBJECT_TYPE_WINTERTREE, "../meshes/WinterTree.fbx");
	MeshContainer::LoadMesh(GAME_OBJECT_TYPE_WORLD_SCENE_SHARK, "../meshes/shark_fin.fbx");

	MeshContainer::LoadMesh(GAME_OBJECT_TYPE_UNIT, "../DudeMesh11.fbx");

	MeshContainer::LoadMesh(GAME_OBJECT_TYPE_WEAPON_SWORD, "../meshes/sword.fbx");
	MeshContainer::LoadMesh(GAME_OBJECT_TYPE_WEAPON_HAMMER, "../meshes/weapon_maul.fbx");
	MeshContainer::LoadMesh(GAME_OBJECT_TYPE_WEAPON_BOMB, "../meshes/weapon_bomb.fbx");

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

	MeshContainer::LoadMesh(GAME_OBJECT_TYPE_TRAP_FIRE, "../meshes/trap_fire.fbx");
	MeshContainer::LoadMesh(GAME_OBJECT_TYPE_TRAP_FREEZE, "../meshes/trap_freeze.fbx");
	MeshContainer::LoadMesh(GAME_OBJECT_TYPE_TRAP_SPRING, "../meshes/TrapPlate.fbx");
	MeshContainer::LoadMesh(GAME_OBJECT_TYPE_TRAP_SPIKES, "../meshes/trap_spikes.fbx");

	MeshContainer::LoadMesh(GAME_OBJECT_TYPE_POWERUP_HEALTH_PACK, "../meshes/hexagon_tile5.fbx");

	MeshContainer::LoadMesh(GAME_OBJECT_TYPE_BARREL_STONES, "../meshes/barrel_rock.fbx");
	MeshContainer::LoadMesh(GAME_OBJECT_TYPE_BARREL_BARREL, "../meshes/Barrel.fbx");

	MeshContainer::LoadMesh(GAME_OBJECT_TYPE_TREE_LEAVES, "../meshes/tree_leaves.fbx");
	MeshContainer::LoadMesh(GAME_OBJECT_TYPE_TREE_TRUNK, "../meshes/tree_trunk.fbx");
	MeshContainer::LoadMesh(GAME_OBJECT_TYPE_TREE_ROCK, "../meshes/tree_rock.fbx");

	MeshContainer::LoadMesh(GAME_OBJECT_TYPE_PINE_LEAVES, "../meshes/pine_tree_leaves.fbx");
	MeshContainer::LoadMesh(GAME_OBJECT_TYPE_PINE_TRUNK, "../meshes/pine_tree_trunk.fbx");

	MeshContainer::LoadMesh(GAME_OBJECT_TYPE_DESERT_CACTUS, "../meshes/desert_cactus.fbx");
	MeshContainer::LoadMesh(GAME_OBJECT_TYPE_DESERT_BOX, "../meshes/desert_box.fbx");
	MeshContainer::LoadMesh(GAME_OBJECT_TYPE_DESERT_SKULL, "../meshes/desert_skull.fbx");

	MeshContainer::LoadMesh(GAME_OBJECT_TYPE_TOWER_TOWER, "../meshes/tower_tower.fbx");
	MeshContainer::LoadMesh(GAME_OBJECT_TYPE_TOWER_CAGE, "../meshes/tower_cage.fbx");

	MeshContainer::LoadMesh(GAME_OBJECT_TYPE_MESH_CRYSTAL_FORMATION, "../meshes/crystal_formation.fbx");
	MeshContainer::LoadMesh(GAME_OBJECT_TYPE_MESH_CAGE, "../meshes/Cage.fbx");
	MeshContainer::LoadMesh(GAME_OBJECT_TYPE_MESH_COWSKULL, "../meshes/CowSkull.fbx");

	MeshContainer::LoadMesh(GAME_OBJECT_TYPE_MESH_BOX_BOXES, "../meshes/box_box.fbx");
	MeshContainer::LoadMesh(GAME_OBJECT_TYPE_MESH_BOX_PLANKS, "../meshes/box_planks.fbx");
}