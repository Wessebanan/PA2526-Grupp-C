#include "ecs.h"
#include "gameRendering/InitMesh.h"
#include "gameRendering/PlaceMesh.h"

#include "gameAI/InitArmy.h"
#include "gameAI/InitGrid.h"
#include "AIGlobals.h"

#include "Input/InitInput.h"
#include "Input/InitInterpreter.h"

#include "gameUtility/InitCamera.h"

#include "gameSceneObjects/InitSceneObjectsh.h"
#include "gameSceneObjects/InitBiomes.h"
#include "gameUtility/UtilityEcsFunctions.h"

#include "gameAudio/InitAudio.h"

#include "Physics/InitPhysics.h"

#include <time.h>

using namespace ecs;								  

int main()
{

	srand(time(0));

	EntityComponentSystem ecs;

	// Tiles + sceneobjects + units + camera
	ecs.reserveComponentCount<ecs::components::TransformComponent>(144 + 12 + 12 + 1);
	ecs.reserveComponentCount<ecs::components::ColorComponent>(144 + 12 + 12);
	ecs.reserveComponentCount<ecs::components::TileComponent>(144);

	InitSound(ecs);

	ecs.createSystem<ecs::systems::PathfindingStateSystem>();
	ecs.createSystem<ecs::systems::IdleStateSystem>();
	ecs.createSystem<ecs::systems::MoveStateSystem>();
	ecs.createSystem<ecs::systems::SwitchStateSystem>();
	ecs.createSystem<ecs::systems::DynamicMovementSystem>();
	ecs.createSystem<ecs::systems::GroundCollisionSystem>();

	InitInput(ecs);
	InitInterpreter(ecs);
	//CameraFunctions::CreateDevCamera(ecs);


	InitGrid(ecs);
	InitArmy(ecs);
	InitSceneObjects(ecs);


	rendering::SUN_DESC sun_desc;
	sun_desc.Red = 200;
	sun_desc.Green = 200;
	sun_desc.Blue = 200;
	
	sun_desc.Direction	= { 0.8f, -1.0f, 0.0f };
	sun_desc.Position	= { -4.0f, 8.0f, 10.0f };

	sun_desc.NearPlane = 0.1f;
	sun_desc.FarPlane = 100.0f;

	sun_desc.Width = 25.0f;
	sun_desc.Height = 25.0f;

	sun_desc.Resolution = 2048;
	
	rendering::RenderManager* pMng = new rendering::RenderManager;
	pMng->Initialize(sun_desc, 1600, 900, "D3D11");
	InitCamera(ecs);

	ModelLoader::Mesh **pp_meshes = InitMesh(ecs, pMng);
	
	InitPhysics(ecs, pp_meshes);

	graphics::PresentWindow* pWnd = pMng->GetPresentWindow();

	PlaceMesh(ecs, pMng);


	// to get components in the loop
	ecs::ComponentIterator itt;

	itt = ecs.getAllComponentsOfType(ecs::components::CameraComponent::typeID);
	ecs::components::CameraComponent* p_cam_comp = (ecs::components::CameraComponent*)itt.next();

	ecs::components::DynamicMovementComponent dyn_move;
	ecs::TypeFilter dr_philter;
	dr_philter.addRequirement(ecs::components::UnitComponent::typeID);
	ecs::EntityIterator ittt = ecs.getEntititesByFilter(dr_philter);
	ecs::events::ChangeUserStateEvent cool_bean;
	ecs::events::ChangeUserStateEvent cool_bean2;
	ecs::events::ChangeUserStateEvent cool_bean3;
	ecs::events::ChangeUserStateEvent cool_bean4;
	for (FilteredEntity p_entity : ittt.entities)
	{
		
		ecs.createComponent<components::DynamicMovementComponent>(p_entity.entity->getID(), dyn_move);
		
	}

	cool_bean.newState = STATE::MOVE;
	cool_bean.playerId = PLAYER::PLAYER1;
	ecs.createEvent(cool_bean);

	cool_bean2.newState = STATE::MOVE;
	cool_bean2.playerId = PLAYER::PLAYER2;
	ecs.createEvent(cool_bean2);

	cool_bean3.newState = STATE::MOVE;
	cool_bean3.playerId = PLAYER::PLAYER3;
	ecs.createEvent(cool_bean3);

	cool_bean4.newState = STATE::MOVE;
	cool_bean4.playerId = PLAYER::PLAYER4;
	ecs.createEvent(cool_bean4);

	ecs::ComponentIterator itttt = ecs.getAllComponentsOfType(ecs::components::ArmyComponent::typeID);
	ecs::components::ArmyComponent* army_comp;
	BaseComponent* p_base_comp;
	components::TransformComponent* army_transform;
	XMMATRIX some_world_matrix_from_army_trans_comp;
	//p_base_comp = itttt.next();
	
	pMng->SetViewMatrix(p_cam_comp->viewMatrix);
	pWnd->Show();
	ecs.update(0.1f);
	while (pWnd->IsOpen())
	{
		if (!pWnd->Update())
		{
			pMng->Clear(0.2f, 0.1f, 0.1f);


			pMng->SetViewMatrix(p_cam_comp->viewMatrix);
			int int_itt = 0;
			ecs::ComponentIterator itttt = ecs.getAllComponentsOfType(ecs::components::ArmyComponent::typeID);
			while (p_base_comp = itttt.next())
			{
				army_comp = static_cast<ecs::components::ArmyComponent*>(p_base_comp);
				for (int i = 0; i < army_comp->unitIDs.size(); i++)
				{
					army_transform = ecs.getComponentFromEntity<components::TransformComponent>(army_comp->unitIDs.at(i));
					some_world_matrix_from_army_trans_comp = UtilityEcsFunctions::GetWorldMatrix(*army_transform);
					DirectX::XMFLOAT4X4* pMatrices = (DirectX::XMFLOAT4X4*)pMng->GetTechniqueModelBuffer(rendering::RENDER_TRANSFORMATION);
					DirectX::XMStoreFloat4x4(&pMatrices[int_itt++], some_world_matrix_from_army_trans_comp);
				}
			}
			
			
			pMng->Draw();
			pWnd->Present();

			ecs.update(0.02f);
		}
	}
	
	for (int i = 0; i < Mesh::N_MESHES; i++) delete pp_meshes[i];
	delete[] pp_meshes;

	pMng->Destroy();
	return 0;
}