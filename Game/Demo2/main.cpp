#include "ecs.h"
#include "rendering/RenderManager.h"
#include "Mesh.h"
#include "GridFunctions.h"
#include "CameraFunctions.h"
#include "InitInputHandler.h"
#include "..//..//InputInterpreter/includes/InterpretWebEvents.h"
#include "AIFunctions.h"
#include "UISystems.h"


int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);


	UINT grid_width = 12;
	UINT grid_height = 12;
	UINT nr_tiles = grid_height * grid_width;
	UINT transf_comp_count = nr_tiles + 1 + 12;

	using namespace ecs;
	ecs::EntityComponentSystem ecs;
	{
		ecs::ECSDesc ecsDesc;
		ecs::CompTypeMemDesc ecsMemDesc[] =
		{
			{ components::TileComponent::typeID, components::TileComponent::size, (nr_tiles)},
			{ components::TransformComponent::typeID, components::TransformComponent::size, transf_comp_count},
		};
		ecsDesc.compTypeCount = 2;
		ecsDesc.compTypeMemDescs = ecsMemDesc;
		ecsDesc.systemLayerCount = 10;
		ecs.initialize(ecsDesc);
	}
	GridFunctions::CreateGrid(ecs, grid_width, grid_height, 1.0f);


	initInputECS(ecs, new InputBackend());


	CameraFunctions::CreateDevCamera(ecs);

	ecs.createSystem<ecs::systems::UpdateCameraSystem>();


	AIFunctions::CreatePlayerArmies(ecs);
	ecs.createSystem<systems::SwitchStateSystem>();
	ecs.createSystem<systems::IdleStateSystem>();
	ecs.createSystem<systems::MoveStateSystem>();

	//ecs.createSystem<systems::ChangeFSMSystem>();

	//events::ChangeUserStateEvent preset_move_event;
	//preset_move_event.playerId = PLAYER1;
	//preset_move_event.newState = STATE::MOVE;
	//ecs.createEvent(preset_move_event);

	Direct2D* d2d = new Direct2D();
	//init::InitUISystems(ecs, &d2d);

	using namespace rendering;
	using namespace DirectX;

	RenderManager mng;
	mng.Initialize(1280, 720, "D3D11");


	ModelLoader::Mesh tile2("../hexagon_tile2.fbx");
	ModelLoader::Mesh tile3("../hexagon_tile3.fbx");
	ModelLoader::Mesh tile("../hexagon_tile.fbx");
	ModelLoader::Mesh dude("../dudeMesh0.fbx");

	int mesh_tile; // tile
	{
		VERTEX_BUFFER_DATA vertex_data = { NULL };
		vertex_data.VertexCount = tile3.GetVertexPositionVector()->size();
		vertex_data.pVertexData = tile3.GetVertexPositionVector()->data();

		INDEX_BUFFER_DATA index_data = { NULL };
		index_data.IndexCount = tile3.GetIndexVector()->size() * 4;
		index_data.pIndexData = tile3.GetIndexVector()->data();

		mesh_tile = mng.CreateMesh(
			&vertex_data,
			&index_data);
	}

	int mesh_dude; // dude
	{
		VERTEX_BUFFER_DATA vertex_data = { NULL };
		vertex_data.VertexCount = dude.GetVertexPositionVector()->size();

		vertex_data.pVertexData = dude.GetVertexPositionVector()->data();
		vertex_data.pTextureCoordData = dude.GetUVVector()->data();

		INDEX_BUFFER_DATA index_data = { NULL };
		index_data.IndexCount = dude.GetIndexVector()->size() * 4;
		index_data.pIndexData = dude.GetIndexVector()->data();

		mesh_dude = mng.CreateMesh(
			&vertex_data,
			&index_data);
	}


	struct float4
	{
		float x, y, z;
		uint32_t color;
	};



	TECHNIQUE_HEAP_LAYOUT_DESC desc[RENDER_TECHNIQUES_COUNT] = { 0 };

	// Default Technique will render 'count' meshes in white
	MODEL_LAYOUT_DESC m_desc[2];
	m_desc[0].InstanceCount = nr_tiles;
	m_desc[0].MeshIndex = mesh_tile;

	m_desc[1].InstanceCount = 12;
	m_desc[1].MeshIndex = mesh_dude;

	desc[RENDER_DEFAULT].PerInstanceByteWidth = sizeof(float4);
	desc[RENDER_DEFAULT].pModelLayout = &m_desc[0];
	desc[RENDER_DEFAULT].ModelLayoutCount = 1;

	desc[RENDER_TRANSFORMATION].PerInstanceByteWidth = sizeof(float4) * 4;
	desc[RENDER_TRANSFORMATION].pModelLayout = &m_desc[1];
	desc[RENDER_TRANSFORMATION].ModelLayoutCount = 1;

	mng.CreateModelHeap(desc);

	graphics::PresentWindow* pWnd = mng.GetPresentWindow();

	float4* pTilePosition = (float4*)mng.GetTechniqueModelBuffer(RENDER_DEFAULT);
	XMFLOAT4X4* p_unit_pos = (XMFLOAT4X4*)mng.GetTechniqueModelBuffer(RENDER_TRANSFORMATION);

	UINT index = 0;
	ecs::ComponentIterator itt;
	
	itt = ecs.getAllComponentsOfType(ecs::components::TileComponent::typeID);
	ecs::components::TileComponent* tileComp;
	while (tileComp = (ecs::components::TileComponent*)itt.next())
	{
		ecs::components::TransformComponent* trComp = ecs.getComponentFromEntity<ecs::components::TransformComponent>(tileComp->getEntityID());

		pTilePosition[index].x = trComp->position.x;
		pTilePosition[index].y = trComp->position.y;
		pTilePosition[index].z = trComp->position.z;

		int random = rand() % 101;
		int color_offset = -50 + random;
		switch (tileComp->tileType)
		{
		case TileTypes::GRASS:
			pTilePosition[index].color = PACK(0, 150 + color_offset, 0, 0);
			break;
		case TileTypes::STONE:
			pTilePosition[index].color = PACK(50 + color_offset, 50 + color_offset, 50 + color_offset, 0);
			break;
		case TileTypes::WATER:
			pTilePosition[index].color = PACK(0, 0, 200 + color_offset, 0);
			break;
		case TileTypes::UNDEFINED:
			pTilePosition[index].color = PACK(0, 0, 0, 255);
			break;
		default:
			pTilePosition[index].color = PACK(255, 255, 255, 255);
			break;
		}

		index++;
	}

	int armyIndex = 0;
	itt = ecs.getAllComponentsOfType(ecs::components::ArmyComponent::typeID);
	ecs::components::ArmyComponent* armComp;
	while (armComp = (ecs::components::ArmyComponent*)itt.next())
	{
		// over alla units in the army
		for (size_t i = 0; i < 3; i++)
		{
			ecs::components::TransformComponent* trComp = ecs.getComponentFromEntity<ecs::components::TransformComponent>(armComp->unitIDs[i]);

			XMMATRIX world = XMMatrixIdentity();
			world *= XMMatrixScaling(trComp->scale.x, trComp->scale.y, trComp->scale.z);
			world *= XMMatrixRotationRollPitchYaw(trComp->rotation.x, trComp->rotation.y, trComp->rotation.z);
			world *= XMMatrixTranslation(trComp->position.x, trComp->position.y, trComp->position.z);
			


			XMStoreFloat4x4(&p_unit_pos[armyIndex], world);

			armyIndex++;
		}
	}

	//for (size_t i = 0; i < 4; i++)
	//{
	//	for (size_t j = 0; j < 3; j++)
	//	{
	//		pTilePosition[index].x = (i % 2) * gridWidth + j;
	//		pTilePosition[index].y = 10;
	//		pTilePosition[index].z = ((i % 2)) * gridHeight + j;
	//		pTilePosition[index].w = 1.0f / i;

	//		index++;
	//	}

	//}


	itt = ecs.getAllComponentsOfType(ecs::components::CameraComponent::typeID);
	components::CameraComponent* camComp;
	camComp = (components::CameraComponent*)itt.next();
	mng.SetViewMatrix(camComp->viewMatrix);


	itt = ecs.getAllComponentsOfType(ecs::components::UserCommandComponent::typeID);
	components::UserCommandComponent* commComp;
	commComp = (components::UserCommandComponent*)itt.next();

	float movement = 0.0f;
	pWnd->Show();

	ecs.update(0.1f);
	while (pWnd->IsOpen())
	{
		if (!pWnd->Update())
		{
			mng.Clear(0.2f,0.1f,0.1f);

			float moveSpeed = 0.01f;

			{
				itt = ecs.getAllComponentsOfType(ecs::components::UserCommandComponent::typeID);
				UserCommandComponent* ucComp = (ecs::components::UserCommandComponent*)itt.next();

				if (ucComp)
				{
					for (size_t i = 0; i < 4; i++)
					{
						if (ucComp->userCommands[i].mCommand == "move"/* && mCurrStates[i] != STATE::MOVE*/)
						{
							// change state component
							events::ChangeUserStateEvent cus_event;
							cus_event.newState = STATE::MOVE;
							cus_event.playerId = (PLAYER)i;

							//mCurrStates[i] = STATE::MOVE;

							ecs.createEvent(cus_event);
						}
						else if (ucComp->userCommands[i].mCommand == "idel"/* && mCurrStates[i] != STATE::IDLE*/)
						{
							// change state component
							events::ChangeUserStateEvent cus_event;
							cus_event.newState = STATE::IDLE;
							cus_event.playerId = (PLAYER)i;

							//mCurrStates[i] = STATE::IDLE;

							ecs.createEvent(cus_event);
						}
					}
				}
			}

			//for (size_t i = 0; i < 4; i++)
			//{
			//	if (commComp->userCommands[i].mCommand == "move")
			//	{
			//		int unitToMove = i*3;

			//		movement = pTilePosition[nr_tiles + (unitToMove)].y;
			//		movement -= 0.1f;
			//		if (movement <= 0.0f)
			//			movement = 40.0f;


			//		pTilePosition[nr_tiles + unitToMove].y = movement;
			//		pTilePosition[nr_tiles + unitToMove+1].y = movement;
			//		pTilePosition[nr_tiles + unitToMove+2].y = movement;
			//	}
			//	

			//	
			//}

			armyIndex = 0;
			itt = ecs.getAllComponentsOfType(ecs::components::ArmyComponent::typeID);
			while (armComp = (ecs::components::ArmyComponent*)itt.next())
			{
				// over alla units in the army
				for (size_t i = 0; i < 3; i++)
				{
					ecs::components::TransformComponent* trComp = ecs.getComponentFromEntity<ecs::components::TransformComponent>(armComp->unitIDs[i]);

					XMMATRIX world = XMMatrixIdentity();
					world *= XMMatrixScaling(trComp->scale.x, trComp->scale.y, trComp->scale.z);
					world *= XMMatrixRotationRollPitchYaw(trComp->rotation.x, trComp->rotation.y, trComp->rotation.z);
					world *= XMMatrixTranslation(trComp->position.x, trComp->position.y, trComp->position.z);



					XMStoreFloat4x4(&p_unit_pos[armyIndex], world);




					armyIndex++;
				}
			}

			


			mng.SetViewMatrix(camComp->viewMatrix);

			mng.Draw();
			pWnd->Present();

			ecs.update(0.1f);
		}
	}

	mng.Destroy();

	return 0;

}