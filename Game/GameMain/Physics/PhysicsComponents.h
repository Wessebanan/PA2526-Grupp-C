#pragma once
#include "ecs.h"
#include <DirectXMath.h>
#include "Mesh.h"
//#include "AABB.h"
//#include "BoundingSphere.h"
//#include "OBB.h"
#include <DirectXCollision.h>

#define COMP(name) struct name : public ecs::ECSComponent<name>

#define DEFAULT_MOVEMENT_FORCE 500.0f
#define DEFAULT_DECELERATION 10.0f
#define DEFAULT_MAX_VELOCITY 100.0f
#define DEFAULT_WEIGHT 50.0f
#define DEFAULT_GRAVITY 9.82f

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
			float mMaxVelocity		= DEFAULT_MAX_VELOCITY;
			float mMovementForce	= DEFAULT_MOVEMENT_FORCE;
			float mDeceleration		= DEFAULT_DECELERATION;
			float mWeight			= DEFAULT_WEIGHT;
			float mGravity			= DEFAULT_GRAVITY;

			DirectX::XMFLOAT3 mDirection	= DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
			DirectX::XMFLOAT3 mForce		= DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
			DirectX::XMFLOAT3 mAcceleration = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
			DirectX::XMFLOAT3 mVelocity		= DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
			DirectX::XMFLOAT3 mForward		= DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);

			// Assuming objects are equal in
			// density all over.
			DirectX::XMFLOAT3 mMassCenter;

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
			//DirectX::XMFLOAT3 mVertices[8] = { DirectX::XMFLOAT3(0, 0, 0) };
			DirectX::BoundingOrientedBox mOBB;
			//OBB mOBB;
			// Center Position is the middle of the box, for distance calculation.
			// Important to apply transform to this point as well.
			//DirectX::XMFLOAT3 mCenterPos = DirectX::XMFLOAT3(0, 0, 0);

			// Storing last y values to avoid unneccesary checks.
			float mLastY = INFINITY;
			float mLastTileY = INFINITY;
		};
		
		/*
		* Object collision component is in its current state
		* a non-adaptive AABB, since there needs to be vertex
		* groups and animations to begin using BVHs and 
		* adaptive bounding volumes.
		*/
		COMP(ObjectCollisionComponent)
		{
			DirectX::BoundingBox mAABB;
			//AABB mAABB;
			//DirectX::XMFLOAT3 mMin		= DirectX::XMFLOAT3(0, 0, 0);
			//DirectX::XMFLOAT3 mMax		= DirectX::XMFLOAT3(0, 0, 0);
			//DirectX::XMFLOAT3 mCenter	= DirectX::XMFLOAT3(0, 0, 0);

			// States if the last movement resulted in collision
			// and needs to be reverted.
			bool mIntersect = false;
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

