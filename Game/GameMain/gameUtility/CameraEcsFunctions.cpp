#include "CameraEcsFunctions.h"
#include "ecs.h"
#include "GlobalsCamera.h"

using namespace DirectX;
using namespace ecs;
using namespace ecs::components;

namespace CameraEcsFunctions
{
	void initComponents(TransformComponent& transf_comp, CameraComponent& cam_comp) 
	{
		//Initialize components
		TransformComponent transform = transf_comp;
		CameraComponent camera = cam_comp;

		transform.rotation = CameraDefines::originalRotation;
		transform.scale = CameraDefines::originalScale;
		camera.up = CameraDefines::originalUp;
		camera.forward.x = camera.target.x - transform.position.x;
		camera.forward.y = camera.target.y - transform.position.y;
		camera.forward.z = camera.target.z - transform.position.z;

		camera.right = CameraDefines::originalRight;
		XMVECTOR cam_pos = XMVectorSet(transform.position.x, transform.position.y, transform.position.z, 0.0f);
		XMVECTOR target = XMLoadFloat4(&camera.target);
		XMVECTOR up = XMLoadFloat4(&camera.up);
		XMMATRIX view = XMLoadFloat4x4(&camera.viewMatrix);
		XMMATRIX projection = XMLoadFloat4x4(&camera.projectionMatrix);

		//Set the view and projection matrix in the CameraComponent.
		view = XMMatrixLookAtLH(cam_pos, target, up);
		projection = XMMatrixPerspectiveFovLH(CameraDefines::fovAngle, CameraDefines::aspectRatio, CameraDefines::nearPlane, CameraDefines::farPlane);

		//Store the values in the component.
		XMStoreFloat4(&camera.target, target);
		XMStoreFloat4(&camera.up, up);
		XMStoreFloat4x4(&camera.viewMatrix, view);
		XMStoreFloat4x4(&camera.projectionMatrix, projection);


		transf_comp = transform;
		cam_comp = camera;
	}

	void CreateDevCamera(TransformComponent& transf_comp, CameraComponent& cam_comp)
	{
		GridProp* p_gp = GridProp::GetInstance();
		int2 arena_size = p_gp->GetSize();
		//Initialize components
		TransformComponent transform;
		CameraComponent camera;
		//transform.position = {10.0f, 1.0f, 0.0f};
		switch (p_gp->mCurrentMap)
		{
		case 0:
			transform.position = { ((arena_size.y * TILE_RADIUS * 1.5f) / 2.0f) - TILE_RADIUS, (arena_size.x + arena_size.y) / 2.5f, TILE_RADIUS * 1 };
			camera.target = { ((arena_size.y * TILE_RADIUS * 1.5f) / 2.0f) - TILE_RADIUS , 0.0f, (((arena_size.x * TILE_RADIUS * 2) / 2.0f) - TILE_RADIUS) / 2.0f + TILE_RADIUS, 0.0f };
			break;
		case 1:
			transform.position = { ((arena_size.y * TILE_RADIUS * 1.5f) / 2.0f) - TILE_RADIUS, (arena_size.x + arena_size.y) / 3.0f, TILE_RADIUS * 2 };
			camera.target = { ((arena_size.y * TILE_RADIUS * 1.5f) / 2.0f) - TILE_RADIUS , 0.0f, (((arena_size.x * TILE_RADIUS * 2) / 2.0f) - TILE_RADIUS) / 2.0f, 0.0f };
			break;
		case 2:
			transform.position = { ((arena_size.y * TILE_RADIUS * 1.5f) / 2.0f) - TILE_RADIUS, (arena_size.x + arena_size.y) / 3.0f, TILE_RADIUS * 3 };
			camera.target = { ((arena_size.y * TILE_RADIUS * 1.5f) / 2.0f) - TILE_RADIUS , 0.0f, (((arena_size.x * TILE_RADIUS * 2) / 2.0f) - TILE_RADIUS) / 2.0f + TILE_RADIUS, 0.0f };
			break;
		default:
			transform.position = { ((arena_size.y * TILE_RADIUS * 1.5f) / 2.0f) - TILE_RADIUS, (arena_size.x + arena_size.y) / 3.0f, TILE_RADIUS * 3 };
			camera.target = { ((arena_size.y * TILE_RADIUS * 1.5f) / 2.0f) - TILE_RADIUS , 0.0f, (((arena_size.x * TILE_RADIUS * 2) / 2.0f) - TILE_RADIUS) / 2.0f + TILE_RADIUS, 0.0f };
			break;
		}

		initComponents(transform, camera);

		transf_comp = transform;
		cam_comp = camera;
	}
	void CreateDynamicCamera(TransformComponent& transf_comp, CameraComponent& cam_comp)
	{
		GridProp* p_gp = GridProp::GetInstance();
		int2 arena_size = p_gp->GetSize();
		//Initialize components
		TransformComponent transform;
		CameraComponent camera;


		transform.position = 
		{ 
			((arena_size.y * TILE_RADIUS * 1.5f) / 2.0f) - TILE_RADIUS, 
			(arena_size.x + arena_size.y) / 3.0f,
			TILE_RADIUS * 2 
		};
		camera.target = 
		{ 
			((arena_size.y * TILE_RADIUS * 1.5f) / 4.0f) - TILE_RADIUS ,
			-7.5f, 
			(((arena_size.x * TILE_RADIUS * 2) / 2.0f) + TILE_RADIUS*10) / 3.0f,
			0.0f 
		}; //To the left in the middle.


		initComponents(transform, camera);

		transf_comp = transform;
		cam_comp = camera;

	}
	void InitializeCamera(ecs::EntityComponentSystem& rECS, ecs::Entity& rCameraEntity)
	{
		//Initialize components
		TransformComponent* p_camera_transform = rECS.getComponentFromEntity<TransformComponent>(rCameraEntity.getID());
		CameraComponent* p_camera_component = rECS.getComponentFromEntity<CameraComponent>(rCameraEntity.getID());
		OverlookCameraSystemComponent overlook_comp;
		InitOverlookCameraComponent(overlook_comp);
		rECS.createComponent(rCameraEntity.getID(), overlook_comp);
		p_camera_transform->position = overlook_comp.startPos;
		p_camera_component->target = overlook_comp.target;
		initComponents(*p_camera_transform, *p_camera_component);
	}

	void InitOverlookCameraComponent(OverlookCameraSystemComponent& rOverlookComponent)
	{
		GridProp* p_gp = GridProp::GetInstance();
		int2 arena_size = p_gp->GetSize();
		// Position should be abover the map
		rOverlookComponent.startPos =
		{
			((arena_size.y * TILE_RADIUS * 1.5f) / 2.0f) - TILE_RADIUS,
			(arena_size.x + arena_size.y) / 2.0f,
			(((arena_size.x * TILE_RADIUS * 2) / 2.0f) - TILE_RADIUS) / 1.1f - (TILE_RADIUS + 3.0f)
		};

		rOverlookComponent.target =
		{
			((arena_size.y * TILE_RADIUS * 1.5f) / 2.0f) - TILE_RADIUS ,
			0.0f,
			(((arena_size.x * TILE_RADIUS * 2) / 2.0f) - TILE_RADIUS) / 1.0f - (TILE_RADIUS + 3.0f),
			0.0f
		};
	}
	void InitDynamicCameraComponent(DynamicCameraSystemComponent& rDynamicComponent)
	{
		GridProp* p_gp = GridProp::GetInstance();
		int2 arena_size = p_gp->GetSize();

		rDynamicComponent.startPos =
		{
			((arena_size.y * TILE_RADIUS * 1.5f) / 2.0f) - TILE_RADIUS,
			(arena_size.x + arena_size.y) / 3.0f,
			TILE_RADIUS * 2
		};
		//rDynamicComponent.target =
		//{
		//	((arena_size.y * TILE_RADIUS * 1.5f) / 4.0f) - TILE_RADIUS ,
		//	-7.5f,
		//	(((arena_size.x * TILE_RADIUS * 2) / 2.0f) + TILE_RADIUS * 10) / 3.0f,
		//	0.0f
		//}; //To the left in the middle.
		rDynamicComponent.target =
		{
			TILE_RADIUS * 3.0f,
			-7.5f,
			TILE_RADIUS * 20.0f,
			0.0f
		}; //To the left in the middle.
	}

	void InitArmyZoomCameraComponent(ArmyZoomCameraSystemComponent& rArmyZoomComponent)
	{
		GridProp* p_gp = GridProp::GetInstance();
		int2 arena_size = p_gp->GetSize();

		rArmyZoomComponent.startPos =
		{
			((arena_size.y * TILE_RADIUS * 1.5f) / 2.0f) - TILE_RADIUS,
			(arena_size.x + arena_size.y) / 2.0f,
			(((arena_size.x * TILE_RADIUS * 2) / 2.0f) - TILE_RADIUS) / 1.1f - (TILE_RADIUS + 3.0f)
		};
		//rDynamicComponent.target =
		//{
		//	((arena_size.y * TILE_RADIUS * 1.5f) / 4.0f) - TILE_RADIUS ,
		//	-7.5f,
		//	(((arena_size.x * TILE_RADIUS * 2) / 2.0f) + TILE_RADIUS * 10) / 3.0f,
		//	0.0f
		//}; //To the left in the middle.
		rArmyZoomComponent.target =
		{
			TILE_RADIUS * 3.0f,
			-7.5f,
			TILE_RADIUS * 20.0f,
			0.0f
		}; //To the left in the middle.
	}


	XMVECTOR Lerp(const XMVECTOR& v1, const XMVECTOR& v2)
	{
		const float percent = 0.02;
		XMVECTOR vec_lerp = v1 + percent * (v2 - v1);
		return vec_lerp;
	}

	XMFLOAT3 Slerp(const XMFLOAT3& v1, const XMFLOAT3& v2)
	{
		//Initialize variables and store the vectors XMVECTOR:s so that we can work with them.
		XMFLOAT3 return_position;
		float omega;
		const float t = 0.001f;
		XMVECTOR vec_1 = XMLoadFloat3(&v1);
		XMVECTOR vec_2 = XMLoadFloat3(&v2);
		XMVECTOR vec_1_norm = XMVector3Normalize(vec_1);
		XMVECTOR vec_2_norm = XMVector3Normalize(vec_2);
		XMVECTOR new_position;
		XMVECTOR dot;
		//Calculate dot between the two positions on the dome.
		dot = XMVector3Dot(vec_1_norm, vec_2_norm);
		omega = XMVectorGetX(dot);
		omega = asinf(omega);
		//Calculate the new position along the dome using the Slerp algorithm.
		new_position = (sinf((1.0f - t) * omega) * vec_1_norm + sinf(t * omega) * vec_2_norm) / sinf(omega);

		//(1.0f - mT)* vec_1 + mT * vec_2;//

		XMStoreFloat3(&return_position, new_position);
		//return v1;
		return return_position;
	}

	XMVECTOR Nlerp(const XMVECTOR& v1, const XMVECTOR& v2)
	{
		XMVECTOR vec_norm = Lerp(v1, v2);
		vec_norm = XMVector3Normalize(vec_norm);
		return vec_norm;
	}

	void UpdateViewMatrix(ecs::components::CameraComponent& cam, ecs::components::TransformComponent& camTransform)
	{
		XMMATRIX view = XMMatrixIdentity();
		XMVECTOR cam_pos = XMLoadFloat3(&camTransform.position);
		XMVECTOR cam_up = XMLoadFloat4(&cam.up);
		XMVECTOR cam_target = XMLoadFloat4(&cam.target);
		view = XMMatrixLookAtLH(cam_pos, cam_target, cam_up);
		XMStoreFloat4x4(&cam.viewMatrix, view);
	}

}
