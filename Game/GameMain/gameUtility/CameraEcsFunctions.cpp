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
		//Initialize components
		TransformComponent transform;
		transform.position = CameraDefines::originalPosition;
		transform.rotation = CameraDefines::originalRotation;
		transform.scale = CameraDefines::originalScale;
		CameraComponent camera;
		camera.target = CameraDefines::originalTarget;
		camera.up = CameraDefines::originalUp;
		camera.forward = CameraDefines::originalForward;
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
