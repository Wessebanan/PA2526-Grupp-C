#pragma once
#include "RagdollBoneState.h"

class RagdollBone
{
public:
	// Size of bounding box
	DirectX::XMFLOAT3 mVecSize;

	float mMass;
	float mOneOverMass;
	// Coefficient of restitution
	float mCoeffRes;

	RagdollBone* mpParentBone;

	// Connection-to-parent offset and 
	// parent-to-bone offset
	DirectX::XMFLOAT3 mVecJointOffset;
	DirectX::XMFLOAT3 mVecParentOffset;
	// Linear force and angular momentum
	DirectX::XMFLOAT3 mVecForce;
	DirectX::XMFLOAT3 mVecTorque;

	// Original orientation of the bone
	DirectX::XMFLOAT4 mQuatOrientation;

	// Used to make bones return to original orientation
	// Used for slerp interpolation
	float mResolutionRate;

	// Body's inverse world moment of inertia tensor matrix
	DirectX::XMFLOAT3X3 mMatInvWorldInertiaMatrix;

	// Bounding box points in body space + connection-to-parent offset pos
	DirectX::XMFLOAT3 mVecPoints[9];

	RagdollBoneState mState;
};