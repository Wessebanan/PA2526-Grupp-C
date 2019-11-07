#pragma once
#include "RagdollBone.h"
#include "Collision.h"
#include "../../../ModelLoader/ModelLoader/includes/Mesh.h"

#define MAXIMUM_TIME_SLICE 0.01f

class Ragdoll
{
protected:
	ModelLoader::Skeleton* mpSkeleton;
	ModelLoader::Mesh* mpMesh;
	ModelLoader::UniqueSkeletonData* mpUniqueSkeletonData;
	DWORD mNumBones;
	// List of bones
	RagdollBone* mBones;

	// Function to compute inline crossProd
	DirectX::XMVECTOR CrossProduct(DirectX::XMVECTOR* v1, DirectX::XMVECTOR* v2);

	// Function to multiply vector by matrix and add optional translation
	DirectX::XMVECTOR Transform(DirectX::XMVECTOR* vecSrc,
								DirectX::XMMATRIX* matSrc,
								DirectX::XMVECTOR* vecTranslate = nullptr);
	void GetBoundingBoxSize(ModelLoader::Joint* pJoint,
							DirectX::XMFLOAT3* pVecSize,
							DirectX::XMFLOAT3* pVecJointOffset);

	// Build a bone and set its data
	void BuildBoneData(DWORD boneNum, RagdollBone* pParentBone = nullptr);

	// Set gravity and damping forces on joint
	void SetForces(DWORD boneNum,
				   DirectX::XMFLOAT3* vecGravity,
				   float linearDamping,
				   float angularDamping);

	// Integrate (update) bone motion for a set time slice
	void Integrate(DWORD boneNum, float dt);

	//Process collisions
	DWORD ProcessCollisions(DWORD boneNum,
							Collision* pCollision,
							DirectX::XMMATRIX matCollision);

	// Process bone connections
	void ProcessConnections(DWORD boneNum);
	// Transform the state's points
	void TransformPoints(DWORD boneNum);

public:
	Ragdoll();
	~Ragdoll();

	bool Create(ModelLoader::Skeleton* pSkeleton,
				ModelLoader::UniqueSkeletonData* pUniqueSkeletonData,
				ModelLoader::Mesh* mpMesh,
				DirectX::XMMATRIX* pMatInitialTransformation = nullptr);

	void Free();

	void Resolve(float dt,
		float linearDamping = -0.04f,
		float angularDamping = -0.01f,
		DirectX::XMFLOAT3 pVecGravity = DirectX::XMFLOAT3(0.0f, -9.8f, 0.0f),
		Collision* pCollision = nullptr,
		DirectX::XMMATRIX* pMatCollision = nullptr);

	// Rebuild the frame hierarchy
	void RebuildHierarchy();

	// Function to get num of bones and pointer to specific bone
	DWORD GetNumBones();
	RagdollBone* GetBonePtr(DWORD boneNum);
};