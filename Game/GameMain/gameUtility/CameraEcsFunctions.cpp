#include "CameraEcsFunctions.h"
#include "ecs.h"
#include "GlobalsCamera.h"

using namespace DirectX;
using namespace ecs;
using namespace ecs::components;

namespace CameraEcsFunctions
{
	void CreateDevCamera(ecs::EntityComponentSystem& rEcs)
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
//		transform.position = CameraDefines::originalPosition;
		transform.rotation = CameraDefines::originalRotation;
		transform.scale = CameraDefines::originalScale;
//		camera.target = CameraDefines::originalTarget;
		camera.up = CameraDefines::originalUp;
		//camera.forward = CameraDefines::originalForward;
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

		//Create the camera entity
		rEcs.createEntity(transform, camera);
	}
	void CreateDynamicCamera(ecs::EntityComponentSystem& rEcs)
	{
		GridProp* p_gp = GridProp::GetInstance();
		int2 arena_size = p_gp->GetSize();
		//Initialize components
		TransformComponent transform;
		CameraComponent camera;

		transform.position = { ((arena_size.y * TILE_RADIUS * 1.5f) / 2.0f) - TILE_RADIUS, (arena_size.x + arena_size.y) / 3.0f, TILE_RADIUS * 2 };
		camera.target = { ((arena_size.y * TILE_RADIUS * 1.5f) / 2.0f) - TILE_RADIUS , -2.0f, (((arena_size.x * TILE_RADIUS * 2) / 2.0f) + TILE_RADIUS*10) / 2.0f, 0.0f };

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

		//Create the camera entity
		rEcs.createEntity(transform, camera);
	}
}
