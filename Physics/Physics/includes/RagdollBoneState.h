#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

class RagdollBoneState
{
public:
	DirectX::XMFLOAT3 mVecPosition; // Position
	DirectX::XMFLOAT4 mQuatOrientation; // Orientation
	DirectX::XMFLOAT4X4 mMatOrientation; // Orientation

	DirectX::XMFLOAT3 mVecAngularMomentum;


	DirectX::XMFLOAT3 mVecLinearVelocity;
	DirectX::XMFLOAT3 mVecAngularVelocity;

	// 8 transformed world pos points + connection to parent pos and parent-to-bone offset
	DirectX::XMFLOAT3 mVecPoints[10];
	// Bony's inverse world moment of inertia tensor matrix
	DirectX::XMFLOAT4X4 mMatInvWorldInertiaMatrix;
};