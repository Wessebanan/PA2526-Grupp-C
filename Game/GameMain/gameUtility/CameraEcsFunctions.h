#pragma once
#include "CameraComponents.h"
#include "CameraSystems.h"
#include "CameraEvents.h"
#include "ecs.h"

namespace CameraEcsFunctions
{
	/* Creates a camera entity with a CameraComponent and a Transform component in the ECS system.*/
	void CreateDevCamera(TransformComponent& transf_comp, CameraComponent& cam_comp);
	void CreateDynamicCamera(TransformComponent& transf_comp, CameraComponent& cam_comp);
	void InitializeCamera(ecs::EntityComponentSystem& rECS, ecs::Entity& rCameraEntity);
	void InitOverlookCameraComponent(ecs::components::OverlookCameraSystemComponent& rOverlookComponent);

	void InitDynamicCameraComponent(DynamicCameraSystemComponent& rDynamicComponent);
	void InitArmyZoomCameraComponent(ArmyZoomCameraSystemComponent& rArmyZoomComponent);

	DirectX::XMVECTOR Lerp(const DirectX::XMVECTOR& v1, const DirectX::XMVECTOR& v2);
	DirectX::XMFLOAT3 Slerp(const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2);
	DirectX::XMVECTOR Nlerp(const DirectX::XMVECTOR& v1, const DirectX::XMVECTOR& v2);

	void UpdateViewMatrix(ecs::components::CameraComponent& cam, ecs::components::TransformComponent& camTransform);	
}