#include "CameraFunctions.h"

namespace CameraFunctions
{
	void InitDevCamera(ecs::EntityComponentSystem& rEcs)
	{
		//Initialize components
		TransformComponent transform;
		transform.positionY = 10.0f;
		CameraComponent camera;
		XMVECTOR cam_pos = XMVectorSet(transform.positionX, transform.positionY, transform.positionZ, 0.0f);

		//Set the view and projection matrix in the CameraComponent.
		camera.viewMatrix = XMMatrixLookAtLH(cam_pos, camera.target, camera.up);
		float fov_angle = 0.4f * 3.14f;
		float aspect_ratio = 1280.0f / 720.0f;
		float near_plane = 1.0f;
		float far_plane = 1000.0f;
		camera.projectionMatrix = XMMatrixPerspectiveFovLH(fov_angle, aspect_ratio, near_plane, far_plane);

		//Create the camera entity
		rEcs.createEntity(transform, camera);
	}

	void InitCameraSystems(ecs::EntityComponentSystem& rEcs)
	{
		rEcs.createSystem<systems::UpdateCameraSystem>();
	}
}
