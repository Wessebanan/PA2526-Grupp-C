#include "CameraFunctions.h"
#include "ecs.h"
#include <DirectXMath.h>
using namespace DirectX;
using namespace ecs;
using namespace ecs::components;

namespace CameraFunctions
{
	void CreateDevCamera(ecs::EntityComponentSystem& rEcs)
	{
		//Initialize components
		TransformComponent transform;
		transform.position.y = 10.0f;
		CameraComponent camera;
		XMVECTOR cam_pos = XMVectorSet(transform.position.x, transform.position.y, transform.position.z, 0.0f);
		XMVECTOR target = XMLoadFloat4(&camera.target);
		XMVECTOR up = XMLoadFloat4(&camera.up);
		XMMATRIX view = XMLoadFloat4x4(&camera.viewMatrix);
		XMMATRIX projection = XMLoadFloat4x4(&camera.projectionMatrix);
		
		//Set the view and projection matrix in the CameraComponent.
		view = XMMatrixLookAtLH(cam_pos, target, up);
		float fov_angle = 0.4f * 3.14f;
		float aspect_ratio = 1280.0f / 720.0f;
		float near_plane = 1.0f;
		float far_plane = 1000.0f;
		projection = XMMatrixPerspectiveFovLH(fov_angle, aspect_ratio, near_plane, far_plane);

		//Store the values in the component.
		XMStoreFloat4(&camera.target, target);
		XMStoreFloat4(&camera.up, up);
		XMStoreFloat4x4(&camera.viewMatrix, view);
		XMStoreFloat4x4(&camera.projectionMatrix, projection);

		//Create the camera entity
		rEcs.createEntity(transform, camera);
	}

	void CreateCameraSystems(ecs::EntityComponentSystem& rEcs)
	{
		//rEcs.createSystem<systems::UpdateCameraSystem>();
	}
}
