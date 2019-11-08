#include "Ragdoll.h"
using namespace DirectX;

DirectX::XMVECTOR Ragdoll::CrossProduct(DirectX::XMVECTOR* v1, DirectX::XMVECTOR* v2)
{
	return XMVector3Cross(*v1, *v2);
}

DirectX::XMVECTOR Ragdoll::Transform(DirectX::XMVECTOR* vecSrc, DirectX::XMMATRIX* matSrc, DirectX::XMVECTOR* vecTranslate)
{
	XMVECTOR vec_result = XMVector3TransformCoord(*vecSrc, *matSrc);
	if (vecTranslate)
	{
		vec_result += (*vecTranslate);
	}
	return vec_result;
}

// NYI
void Ragdoll::GetBoundingBoxSize(ModelLoader::Joint* pJoint, DirectX::XMFLOAT3* pVecSize, DirectX::XMFLOAT3* pVecJointOffset)
{
	if (pJoint)
	{
		std::vector<unsigned int>* vertex_indices = &pJoint->mConnectedVertexIndices;
		DWORD num_vertices = vertex_indices->size();
		if (num_vertices > 0)
		{
			// Get the vertices and vertex weights encompassed by this bounding box
			XMFLOAT3* vertices = new XMFLOAT3[num_vertices];
			float* weights = new float[num_vertices];
			std::vector<XMFLOAT3>* mesh_vertices = this->mpMesh->GetVertexPositionVector();
			std::vector<float>* mesh_weights = this->mpMesh->GetBlendWeights();

			
			for (int i = 0; i < num_vertices; ++i)
			{
				vertices[i] = (*mesh_vertices)[(*vertex_indices)[i]];
				weights[i] = (*mesh_weights)[(*vertex_indices)[i]];
				// -#_#_#_#_#_#_#_#_#_#_#_#_#_#_#_#_#_#_#_#_#_#_#_#_#
				// -#_#_#_#_#_#_#_#_#_#_#_#_#_#_#_#_#_#_#_#_#_#_#_#_#
				// -#_#_# NOT FINISHED, MISSING CRITICAL STUFF  #_#_#
				// -#_#_#_#_#_#_#_#_#_#_#_#_#_#_#_#_#_#_#_#_#_#_#_#_#
				// -#_#_#_#_#_#_#_#_#_#_#_#_#_#_#_#_#_#_#_#_#_#_#_#_#
			}
			
			delete[] vertices;
			delete[] weights;
		}
		
	}
}

void Ragdoll::BuildBoneData(DWORD boneNum, RagdollBone* pParentBone)
{
	if (boneNum < this->mNumBones)
	{
		RagdollBone* bone = &mBones[boneNum];
		GetBoundingBoxSize(&this->mpSkeleton->joints[boneNum],
						   &bone->mVecSize,
						   &bone->mVecJointOffset);

		bone->mMass = bone->mVecSize.x * bone->mVecSize.y * bone->mVecSize.z;
		// Mass must be > 0.0
		if (bone->mMass == 0.0f)
		{
			bone->mMass = 1.0f;
		}
		bone->mOneOverMass = 1.0f / bone->mMass;

		// Set the coefficient of restitution (higher = more bounce)
		bone->mCoeffRes = 0.4f;

		// Clear force and torque
		bone->mVecForce = XMFLOAT3(0.0f, 0.0f, 0.0f);
		bone->mVecTorque = XMFLOAT3(0.0f, 0.0f, 0.0f);

		// Set the angular resolution rate
		bone->mResolutionRate = 0.05f;

		// Calculate the inverse body inertia matrix
		float xScalar = bone->mVecSize.x * bone->mVecSize.x;
		float yScalar = bone->mVecSize.y * bone->mVecSize.y;
		float zScalar = bone->mVecSize.z * bone->mVecSize.z;

		XMStoreFloat3x3(&bone->mMatInvWorldInertiaMatrix, XMMatrixIdentity());
		bone->mMatInvWorldInertiaMatrix._11 = 1.0f / bone->mMass * (yScalar + zScalar);
		bone->mMatInvWorldInertiaMatrix._22 = 1.0f / bone->mMass * (xScalar + zScalar);
		bone->mMatInvWorldInertiaMatrix._33 = 1.0f / bone->mMass * (xScalar + yScalar);

		// Setup the point's coordinates based on half 
		// the bounding box size
		XMFLOAT3 vecHalfSize;
		vecHalfSize.x = bone->mVecSize.x * 0.5f; 
		vecHalfSize.y = bone->mVecSize.y * 0.5; 
		vecHalfSize.z = bone->mVecSize.z * 0.5f;
		bone->mVecPoints[0] = XMFLOAT3(-vecHalfSize.x, vecHalfSize.y, -vecHalfSize.z);
		bone->mVecPoints[1] = XMFLOAT3(-vecHalfSize.x, vecHalfSize.y, vecHalfSize.z);
		bone->mVecPoints[2] = XMFLOAT3(vecHalfSize.x, vecHalfSize.y, vecHalfSize.z);
		bone->mVecPoints[3] = XMFLOAT3(vecHalfSize.x, vecHalfSize.y, -vecHalfSize.z);
		bone->mVecPoints[4] = XMFLOAT3(-vecHalfSize.x, -vecHalfSize.y, -vecHalfSize.z);
		bone->mVecPoints[5] = XMFLOAT3(-vecHalfSize.x, -vecHalfSize.y, vecHalfSize.z);
		bone->mVecPoints[6] = XMFLOAT3(vecHalfSize.x, -vecHalfSize.y, vecHalfSize.z);
		bone->mVecPoints[7] = XMFLOAT3(vecHalfSize.x, -vecHalfSize.y, -vecHalfSize.z);

		// Set the joint offset (offset to parent bone connection)
		bone->mVecPoints[8] = bone->mVecJointOffset;

		// ¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&
		// ¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&
		// ¤%&-¤%&- POINT OF INSECURITY - the original code uses "matCombined" here, which I am unsure of. ¤%&-¤%&-¤%&-¤%&
		// ¤%&-¤%&-         I have interpreted "matCombined" as the bone offset matrix, may be wrong       ¤%&-¤%&-¤%&-¤%&
		// ¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&
		// ¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&

		// Set the bone's center position based on transformed joint offset coordinates
		XMStoreFloat3(&bone->mState.mVecPosition,
			XMVector3TransformCoord(-1.0f * XMLoadFloat3(&bone->mVecJointOffset), XMLoadFloat4x4(&this->mpUniqueSkeletonData->frameData[boneNum])));

		// Store state matrix-based orientation. Be sure
		// to remove translation values.
		bone->mState.mMatOrientation = this->mpUniqueSkeletonData->frameData[boneNum];
		bone->mState.mMatOrientation._41 = 0.0f;
		bone->mState.mMatOrientation._42 = 0.0f;
		bone->mState.mMatOrientation._43 = 0.0f;

		// Store state quaternion-based orientation
		// We need to inverse it the quaternion due to the fact 
		// that we're using a left-handed coordinate system
		XMStoreFloat4(&bone->mState.mQuatOrientation, XMQuaternionInverse(XMQuaternionRotationMatrix(XMLoadFloat4x4(&this->mpUniqueSkeletonData->frameData[boneNum]))));

		// Clear angular momentum
		bone->mState.mVecAngularMomentum = XMFLOAT3(0.0f, 0.0f, 0.0f);

		// Clear force and angular velocities
		bone->mState.mVecLinearVelocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
		bone->mState.mVecAngularVelocity = XMFLOAT3(0.0f, 0.0, 0.0f);

		// Clear inverse world inertia tensor matrix
		XMStoreFloat4x4(&bone->mState.mMatInvWorldInertiaMatrix, XMMatrixIdentity());
		
		// Transform points
		for (DWORD j = 0; j < 9; ++j)
		{
			XMStoreFloat3(&bone->mState.mVecPoints[j], 
				Transform(&XMLoadFloat3(&bone->mVecPoints[j]),
				&XMLoadFloat4x4(&bone->mState.mMatOrientation),
				&XMLoadFloat3(&bone->mState.mVecPosition)));
		}

		// Get the difference in orientations from parent bone
		// to bone. This is used to later slerp for angular 
		// resolution. The difference (C) from A to B 
		// is calculated as: C = inverse(A) * B
		if (pParentBone)
		{
			XMStoreFloat4(&bone->mQuatOrientation,
				XMQuaternionInverse(XMLoadFloat4(&pParentBone->mState.mQuatOrientation))* XMLoadFloat4(&bone->mState.mQuatOrientation));
		}
		// If there is a parent bone, then set connection position
		if (bone->mpParentBone = pParentBone)
		{
			// Get the inversed coordinates from the joint connection point
			// to the center of the parent's bone.
			XMMATRIX mat_inv = XMMatrixInverse(nullptr, XMLoadFloat4x4(&pParentBone->mState.mMatOrientation));
			XMVECTOR vec_diff = XMLoadFloat3(&bone->mState.mVecPoints[8]) - XMLoadFloat3(&pParentBone->mState.mVecPosition);
			bone->mVecParentOffset;
			XMStoreFloat3(&bone->mVecParentOffset, Transform(&vec_diff, &mat_inv));
		}
		boneNum += 1;
		// If we still have bones to process
		if (boneNum < this->mpSkeleton->joints.size())
		{
			// Fetch the parent index to send as parent bone for next bone data calculation
			int next_parent_index = this->mpSkeleton->joints[boneNum].mParentIndex;
			BuildBoneData(boneNum, &this->mBones[next_parent_index]);
		}
	}
}

void Ragdoll::SetForces(DWORD boneNum, DirectX::XMFLOAT3* vecGravity, float linearDamping, float angularDamping)
{
	RagdollBone* bone = &this->mBones[boneNum];
	RagdollBoneState* bc_state = &bone->mState;

	// Set gravity and apply damping on force and torque
	XMVECTOR temp_vec_force = XMLoadFloat3(vecGravity) * bone->mMass;
	temp_vec_force += (XMLoadFloat3(&bc_state->mVecLinearVelocity) * linearDamping);
	XMStoreFloat3(&bone->mVecForce, temp_vec_force);
	XMStoreFloat3(&bone->mVecTorque, (XMLoadFloat3(&bc_state->mVecAngularVelocity) * angularDamping));
}
// NYI
void Ragdoll::Integrate(DWORD boneNum, float dt)
{

}
// NYI
bool Ragdoll::ProcessCollisions(DWORD boneNum, Collision* pCollision)
{
	// If there are no objects to check against.
	if (!pCollision || !pCollision->mNumObjects || !pCollision->mObjects)
	{
		return true;
	}
	RagdollBone* bone = &mBones[boneNum];
	
	RagdollBoneState* state = &bone->mState;

	unsigned int collision_count = 0;

	XMFLOAT3 linear_velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 angular_momentum = XMFLOAT3(0.0f, 0.0f, 0.0f);

	// Creating an OBB from its 8 corner points.
	OBB bone_obb;
	bone_obb.CreateFromPoints(bone_obb, 8, state->mVecPoints, sizeof(XMFLOAT3));
	
	CollisionObject* p_object = pCollision->mObjects;
	while (p_object)
	{
		bool collision = bone_obb.Intersects(p_object->mBoundingVolume);

		if (collision)
		{
			CollisionInfo collision_info = p_object->GetCollisionInfo(bone_obb);

			// Additional check for the naïve solution, if no corner is within the checked object.
			if (collision_info.mOverlap > 0.0f)
			{
				collision_count++;

				// Push bone bv out to surface of object.
				XMVECTOR addition = XMVectorScale(XMLoadFloat3(&collision_info.mNormal), collision_info.mOverlap);
				XMVECTOR vec_state_vec_position = XMLoadFloat3(&state->mVecPosition);
				vec_state_vec_position = XMVectorAdd(vec_state_vec_position, addition);
				XMStoreFloat3(&state->mVecPosition, vec_state_vec_position);

				// Get the point's position and velocity
				XMVECTOR vec_p_to_p = vec_state_vec_position - XMLoadFloat3(&bone_obb.Center);
				XMVECTOR vec_p_to_p_velocity = XMLoadFloat3(&state->mVecLinearVelocity) 
					+ XMVector3Cross(XMLoadFloat3(&state->mVecAngularVelocity), vec_p_to_p);

				// Get the point's speed relative to the surface
				float point_speed = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&collision_info.mNormal), vec_p_to_p_velocity));

				// Calculate the impulse force based on the coefficient
				// of restitution, the speed of the point, and the
				// normal of the colliding object.
				float impulse_force = point_speed * (-(1.0f + bone->mCoeffRes));
				float impulse_damping = (1.0f / bone->mMass) + XMVectorGetX(
																	XMVector3Dot(
																		XMVector3Cross(
																			XMVector3TransformNormal(
																				XMVector3Cross(	
																					vec_p_to_p, 
																					XMLoadFloat3(&collision_info.mNormal)), 
																				XMLoadFloat4x4(&state->mMatInvWorldInertiaMatrix)), 
																			vec_p_to_p), 
																		XMLoadFloat3(&collision_info.mNormal)));
				XMVECTOR vec_impulse = XMVectorScale(XMLoadFloat3(&collision_info.mNormal), impulse_force / impulse_damping);
				
				// Add forces to running total
				XMStoreFloat3(&linear_velocity, XMVectorAdd(XMLoadFloat3(&linear_velocity), vec_impulse));
				XMStoreFloat3(&angular_momentum, XMVectorAdd(XMLoadFloat3(&angular_momentum), XMVector3Cross(vec_p_to_p, vec_impulse)));
			}
		}
		p_object = p_object->mNext;
	}

	if (collision_count > 0)
	{
		// Add averaged forces to integrated state.
		XMStoreFloat3(&state->mVecLinearVelocity, XMVectorAdd(XMLoadFloat3(&state->mVecLinearVelocity), XMVectorScale(XMVectorScale(XMLoadFloat3(&linear_velocity), 1.0f / bone->mMass), 1.0f / (float)collision_count));
		XMStoreFloat3(&state->mVecAngularMomentum, XMVectorAdd(XMLoadFloat3(&state->mVecAngularMomentum), XMVectorScale(XMLoadFloat3(&angular_momentum), 1.0f / (float)collision_count)));

		// Calculate angular velocity.
		XMStoreFloat3(&state->mVecAngularVelocity, XMVector3TransformNormal(XMLoadFloat3(&state->mVecAngularMomentum), XMLoadFloat4x4(&state->mMatInvWorldInertiaMatrix)));
	}

	return true;
}
// NYI
void Ragdoll::ProcessConnections(DWORD boneNum)
{
}
// NYI
void Ragdoll::TransformPoints(DWORD boneNum)
{
}

Ragdoll::Ragdoll() : mpMesh(nullptr), mpSkeleton(nullptr), mpUniqueSkeletonData(nullptr),
					 mNumBones(0), mBones(nullptr)
{

}

Ragdoll::~Ragdoll()
{
	Free();
}

bool Ragdoll::Create(ModelLoader::Skeleton* pSkeleton,
	ModelLoader::UniqueSkeletonData* pUniqueSkeletonData,
	ModelLoader::Mesh* mpMesh,
	DirectX::XMMATRIX* pMatInitialTransformation)
{
	if (!(this->mpSkeleton = pSkeleton))
	{
		return false;
	}
	if (!mpMesh || !pUniqueSkeletonData)
	{
		return false;
	}

	// Update the frame hierarchy using transformation passed
	// m_pFrame->UpdateHierarchy(matInitialTransformation);
	// ¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&
	// ¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&-¤%&
	// I am skipping this function and assuming pMatInitialTransformation is nullptr
	// The original code uses DXGIFrames 

	this->mNumBones = pSkeleton->jointCount;
	if (!mNumBones)
		return false;

	mBones = new RagdollBone[this->mNumBones]();

	// Go through and setup each bone's data
	// BuildBoneData is a recursive function
	this->BuildBoneData((DWORD)0);

	mBones[0].mState.mVecAngularMomentum = XMFLOAT3(-1.0f, 0.0f, 0.0f);

	return true;
}

void Ragdoll::Free()
{
	this->mpSkeleton = nullptr;
	this->mpMesh = nullptr;
	this->mpUniqueSkeletonData = nullptr;
	this->mNumBones = 0;
	delete[] mBones;
	mBones = nullptr;
}
// NYI
void Ragdoll::Resolve(float dt, float linearDamping, float angularDamping, DirectX::XMFLOAT3 pVecGravity, Collision* pCollision, DirectX::XMMATRIX* pMatCollision)
{
}
// NYI
void Ragdoll::RebuildHierarchy()
{
}

DWORD Ragdoll::GetNumBones()
{
	return mNumBones;
}

RagdollBone* Ragdoll::GetBonePtr(DWORD boneNum)
{
	if (boneNum < mNumBones)
	{
		return &mBones[boneNum];
	}
	return nullptr;
}
