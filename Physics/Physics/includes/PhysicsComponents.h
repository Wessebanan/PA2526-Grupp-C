#pragma once
#include "ecs.h"
#include <DirectXMath.h>
#include "Mesh.h"
#define COMP(name) struct name : public ecs::ECSComponent<name>

namespace ecs
{
	namespace components
	{
		/*
		* StaticMovementComponent holds velocity
		* and direction which are needed to
		* change the position of an entity.
		*/
		COMP(StaticMovementComponent)
		{
			float mMaxVelocity = 100.0f;
			float mVelocity = 0.0f;
			DirectX::XMFLOAT3 mForward = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
			DirectX::XMFLOAT3 mDirection = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		};
		
		COMP(DynamicMovementComponent)
		{
			float mMaxVelocity = 100.0f;
			float mMaxAcceleration = 10.0f;

			DirectX::XMFLOAT3 mDirection = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
			DirectX::XMFLOAT3 mForce = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
			DirectX::XMFLOAT3 mAcceleration = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
			DirectX::XMFLOAT3 mVelocity = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
			DirectX::XMFLOAT3 mForward = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);

			// Assuming objects are equal in
			// density all over.
			DirectX::XMFLOAT3 mMassCenter;
			float mWeight = 50.0f;

			// If object is on ground.
			bool mOnGround = false;
		};

		/*
		* BoundingSphereComponent holds a description
		* of a bounding sphere, which is necessary
		* to calculate collision. Any entity that
		* should check collision needs this.
		*/
		COMP(BoundingSphereComponent)
		{
			DirectX::XMFLOAT3 mCenter;
			float mRadius;
		};

		/*
		* GroundCollisionComponent is an OBB which only checks
		* against the ground plane for collision. Not using lowest
		* point as objects may rotate which requires recalculation 
		* of the lowest point.
		*/
		COMP(GroundCollisionComponent)
		{
			// Vertices making up the OBB:
			/*
			   6-------7
			  /|      /|
			 / |     / |
			2--|----3  |
			|  4----|--5
			| /     | /
			0-------1		|: y, -: x /: z
			*/
			DirectX::XMFLOAT3 mVertices[8] = { DirectX::XMFLOAT3(0, 0, 0) };
			
			// Center Position is the middle of the box, for distance calculation.
			// Important to apply transform to this point as well.
			DirectX::XMFLOAT3 mCenterPos = DirectX::XMFLOAT3(0, 0, 0);

			
		};
		
		/*
		* Object collision component is in its current state
		* a non-adaptive AABB, since there needs to be vertex
		* groups and animations to begin using BVHs and 
		* adaptive bounding volumes.
		*/
		COMP(ObjectCollisionComponent)
		{
			DirectX::XMFLOAT3 mMin		= DirectX::XMFLOAT3(0, 0, 0);
			DirectX::XMFLOAT3 mMax		= DirectX::XMFLOAT3(0, 0, 0);
			DirectX::XMFLOAT3 mCenter	= DirectX::XMFLOAT3(0, 0, 0);
		};

		/*
		* Temporary mesh component to test with ECS.
		*/
		COMP(MeshComponent)
		{
			ModelLoader::Mesh mMesh;
		};
	} // components
} // ecs

