#pragma once
#include "RagdollBoneState.h"

class RagdollBone
{
public:
	// Size of bounding box
	DirectX::XMFLOAT3 mVecSize;

	float mMass;
	float mOneOvermass;
	// Coefficient of restitution
	float mCoeffRes;

	RagdollBone* mpParentBone;

	DirectX::XMFLOAT3 mVecJointOffset;
	DirectX::XMFLOAT3 mVecParentOffset;
	DirectX::XMFLOAT3 mVecForce;
	DirectX::XMFLOAT3 mVecTorque;

	// Original orientation of the bone
	DirectX::XMFLOAT4 mQuatQuaternion;

	// Used to make bones return to original orientation
	float mResolutionRate;

	// Body's inverse world moment of inertia tensor matrix
	DirectX::XMFLOAT4X4 mMatInvWorldInertiaMatrix;

	// Bounding box points in body space + connection-to-parent offset pos
	DirectX::XMFLOAT3 mVecPoints[9];

	RagdollBoneState mState;
};