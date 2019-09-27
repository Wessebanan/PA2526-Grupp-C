#pragma once
#include <DirectXMath.h>

namespace CameraDefines
{
	constexpr DirectX::XMFLOAT3 originalPosition = { 0.0f, 10.0f, 0.0f };
	constexpr DirectX::XMFLOAT3 originalRotation = { 0.0f, 0.0f, 0.0f };
	constexpr DirectX::XMFLOAT3 originalScale = { 1.0f, 1.0f, 1.0f };

	constexpr DirectX::XMFLOAT4 originalTarget = { 0.0f, 0.0f, 0.0f, 0.0f };
	constexpr DirectX::XMFLOAT4 originalUp = { 0.0f, 1.0f, 0.0f, 0.0f };
	constexpr DirectX::XMFLOAT4 originalForward = { 0.0f, 0.0f, 1.0f, 0.0f };
	constexpr DirectX::XMFLOAT4 originalRight = { 1.0f, 0.0f, 0.0f, 0.0f };

	constexpr float fovAngle = 0.4f * 3.14f;
	constexpr float aspectRatio = 1280.0f / 720.0f;
	constexpr float nearPlane = 1.0f;
	constexpr float farPlane = 1000.0f;
}