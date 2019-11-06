#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

class RagdollBoneState
{
private:
	DirectX::XMFLOAT3 mVecPosition;
	DirectX::XMFLOAT4 mQuatQuaternion;

	DirectX::XMFLOAT3 mVecAngularMomentum;
	DirectX::XMFLOAT3 mVecLinearVelocity;

	// 8 transformed world pos points + connection to parent pos and parent-to-bone offset
	DirectX::XMFLOAT3 mVecPoints[10];
	// Bony's inverse world moment of inertia tensor matrix
	DirectX::XMFLOAT4X4 mMatInvWorldInertiaMatrix;
};